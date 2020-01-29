#include "tree_12x.hpp"

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/minmax.hpp>
#include <graphidx/tree/postorder.hpp>
#include <graphidx/tree/bfs.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/perm.hpp>
#include <graphidx/utils/timer.hpp>


template<typename float_ = float, typename int_ = int>
struct Tree12xStatus
{
    Tree12xStatus(const size_t n, const int_ *forder)
        : n(n),
          forder(forder) {
        y = new float_[n];
        x = new float_[n];
        deriv = new float_[n];
        parent_ = new int_[n];
    }

    static constexpr int_ one =             //  b"100...0"
        (int_(1)<<(8*sizeof(int_)-1));

    inline void divorce(size_t i) { parent_[i] &= ~one; }
    inline void init_parent(size_t i, int_ p) { parent_[i] = p | one; }
    inline bool same(size_t i) { return (parent_[i] & one) != 0; }
    inline int_ parent(size_t i) { return parent_[i] & (~one); }

    ~Tree12xStatus() {
        if (y) delete[] y;
        if (x) delete[] x;
        if (deriv) delete[] deriv;
        if (parent_) delete[] parent_;
    }

    const size_t n = 0;
    const bool is_linear = false;
    float_ *y = nullptr;
    float_ *x = nullptr;
    float_ *deriv = nullptr;
    int_ *parent_ = nullptr;
    const int_ *forder;         // forward order

    size_t iter(const float_ lam, const float_ delta);
};


template<typename float_, typename int_>
size_t
Tree12xStatus<float_, int_>::iter(
    const float_ lam,
    const float_ delta)
{
    const auto root = forder[n-1];

    {   // Timer _ ("deriv init");
        for (size_t i = 0; i < n; i++)
            deriv[i] = x[i] - y[i];
    }

    {   Timer _ (" forward");
        for (size_t i = 0; i < n-1; i++) {
            const auto v = forder[i];
            if (same(v)) {
                const auto p = parent(v);
                deriv[p] += clamp(deriv[v], -lam, +lam);
            }
        }
    }

    size_t changed = 0;
    {   Timer _ (" backward");
        const auto xr = deriv[root] > 0 ? -delta : +delta;
        x[root] += xr;

        for (size_t i = n-1; i > 0; i--) {
            const auto v = forder[i-1];
            if (same(v)) {
                if (deriv[v] > lam) {
                    x[v] -= delta;
                } else if (deriv[v] < -lam) {
                    x[v] += delta;
                } else {
                    x[v] = x[parent(v)];
                    continue;
                }

                const auto p = parent(v);
                if (x[v] < x[p]) {
                    changed++;
                    divorce(v);
                    y[v] += lam;
                    y[p] -= lam;
                } else if (x[v] > x[p]) {
                    changed++;
                    divorce(v);
                    y[v] -= lam;
                    y[p] += lam;
                }
            } else {
                x[v] += deriv[v] < 0 ? +delta : -delta;
            }
        }
    }

    return changed;
}


template<typename float_, typename int_>
void
tree_12x(
    const size_t n,
    const int_ *parent,
    const float_ *y,
    const float_ lam,
    float_ *x,
    const int_ root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder)
{
    Timer _ ("tree_12x:\n");

    std::vector<int> forder;    // forward order
    std::vector<int> iorder;    // inverse of forder
    std::vector<int> fparent;
    ChildrenIndex cidx;
    int_ root = root_;

    if (root < 0) {
        Timer _ ("find_root");
        root = find_root(n, parent);
    }
    {
        Timer _ ("vector::reserve");
        forder.reserve(n);
        if (reorder)
            iorder.reserve(n);
    }
    {
        Timer _ ("children idx");
        cidx.reset(n, parent, root);
    }
    {
        Timer _ ("bfs");
        std::vector<int_> bfs;
        compute_bfs(bfs, cidx);
    }
    {   Timer _ ("dfs postorder\n");
        stack<int_> stack;
        post_order(forder.data(), cidx, stack);
        if (forder[n-1] != root)
            throw std::runtime_error(std::string("tree_12x(): FATAL: ") +
                                     "forder[" + std::to_string(n-1) + "] = " +
                                     std::to_string(forder[n-1]) + " != " +
                                     std::to_string(root) + " = root");
    }
    if (reorder) {
        {   Timer _ ("inverse order");
            invperm(n, iorder.data(), forder.data());
        }
        {   Timer _ ("relabel");
            fparent.reserve(n);
            for (size_t i = 0; i < n; i++)
                fparent[i] = iorder[parent[forder[i]]];
            for (int i = 0; i < int(n); i++)
                forder[i] = i;
            
            // TODO: also relabel y!
        }
    }

    Timer tim ("alloc");
    Tree12xStatus<float_, int_> s (n, forder.data());
    tim.stop();

    float_ min_y, max_y, delta;
    {   Timer _ ("minmax y");
        find_minmax(y, n, min_y, max_y);
        delta = float_((max_y - min_y) * 0.5);
    }

    const auto *pi = reorder ? fparent.data() : parent;
    {   Timer _ ("init x,y,parent");
        const float_ x0 = float_(0.5 * (min_y + max_y));
        for (size_t i = 0; i < n; i++)
            s.x[i] = x0;
        for (size_t i = 0; i < n; i++)
            s.y[i] = y[i];
        for (size_t i = 0; i < n; i++)
            s.init_parent(i, pi[i]);
    }

    {   Timer _ ("iterations:\n");
        for (int k = 0; k < max_iter; k++) {
            size_t changed = 0;
            Timer::log("%2ld ...%s", k+1, print_timings ? "\n" : "");

            delta = float_(0.5*delta);
            {
                TimerQuiet _ (print_timings);
                changed = s.iter(lam, delta);
            }
            if (changed)
                Timer::log("  %'d", changed);
            Timer::log("\n");
        }
    }

    {   Timer _ ("extract x");
        // TODO: take care of reorder
        for (size_t i = 0; i < n; i++)
            x[i] = s.x[i];
    }
}


template
void
tree_12x(
    const size_t n,
    const int *parent,
    const float *y,
    const float lam,
    float *x,
    const int root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder);


template
void
tree_12x(
    const size_t n,
    const int *parent,
    const double *y,
    const double lam,
    double *x,
    const int root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder);
