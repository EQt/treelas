#include "tree_12x.hpp"

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/minmax.hpp>
#include <graphidx/tree/postorder.hpp>
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
    float_ *y = nullptr;
    float_ *x = nullptr;
    float_ *deriv = nullptr;
    int_ *parent_ = nullptr;
    const int_ *forder;
};


template<typename float_ = float, typename int_ = int>
size_t
tree_12x_iter(
    Tree12xStatus<float_, int_> &s,
    const float_ lam,
    const float_ delta)
{
    const auto root = s.forder[s.n-1];

    {   // Timer _ ("deriv init");
        for (size_t i = 0; i < s.n; i++)
            s.deriv[i] = s.x[i] - s.y[i];
    }

    {   Timer _ ("forward");
        for (size_t i = 0; i < s.n-1; i++) {
            const auto v = s.forder[i];
            if (s.same(v)) {
                const auto p = s.parent(v);
                s.deriv[p] += clamp(s.deriv[v], -lam, +lam);
            }
        }
    }

    size_t changed = 0;
    {   Timer _ ("backward");
        const auto xr = s.deriv[root] > 0 ? -delta : +delta;
        s.x[root] += xr;

        for (size_t i = s.n-1; i > 0; i--) {
            const auto v = s.forder[i-1];
            if (s.same(v)) {
                if (s.deriv[v] > lam) {
                    s.x[v] -= delta;
                } else if (s.deriv[v] < -lam) {
                    s.x[v] += delta;
                } else {
                    s.x[v] = s.x[s.parent(v)];
                    continue;
                }

                const auto p = s.parent(v);
                if (s.x[v] < s.x[p]) {
                    changed++;
                    s.divorce(v);
                    s.y[v] += lam;
                    s.y[p] -= lam;
                } else if (s.x[v] > s.x[p]) {
                    changed++;
                    s.divorce(v);
                    s.y[v] -= lam;
                    s.y[p] += lam;
                }
            } else {
                s.x[v] += s.deriv[v] < 0 ? +delta : -delta;
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
    std::vector<int> forder_;
    std::vector<int> iorder;
    std::vector<int> newp;
    forder_.reserve(n);
    if (reorder)
        iorder.reserve(n);
    int_ root = root_ < 0 ? find_root(n, parent) : root_;

    {   Timer _ ("dfs postorder\n");
        post_order(n, parent, root, forder_.data());
        if (forder_[n-1] != root)
            throw std::runtime_error(std::string("tree_12x(): FATAL: ") +
                                     "forder[" + std::to_string(n-1) + "] = " +
                                     std::to_string(forder_[n-1]) + " != " +
                                     std::to_string(root) + " = root");
    }
    if (reorder) {
        {   Timer _ ("inverse order");
            invperm(n, iorder.data(), forder_.data());
        }
        {   Timer _ ("relabel");
            newp.reserve(n);
            for (size_t i = 0; i < n; i++)
                newp[i] = iorder[parent[forder_[i]]];
            for (int i = 0; i < int(n); i++)
                forder_[i] = i;
            // TODO: also relabel y!
        }
    }

    Timer tim ("alloc");
    Tree12xStatus<float_, int_> s (n, forder_.data());
    tim.stop();

    float_ min_y, max_y, delta;
    {   Timer _ ("minmax y");
        find_minmax(y, n, min_y, max_y);
        delta = float_((max_y - min_y) * 0.5);
    }

    const auto *pi = reorder ? newp.data() : parent;
    {   Timer _ ("init x,y,parent");
        const float_ x0 = float_(0.5 * (min_y + max_y));
        for (size_t i = 0; i < n; i++)
            s.x[i] = x0;
        for (size_t i = 0; i < n; i++)
            s.y[i] = y[i];
        for (size_t i = 0; i < n; i++)
            s.init_parent(i, pi[i]);
    }

    {   Timer _ ("Iterations:\n");
        for (int k = 0; k < max_iter; k++) {
            size_t changed = 0;
            Timer::log("%2ld ...%s", k+1, print_timings ? "\n" : "");

            delta = float_(0.5*delta);
            {
                TimerQuiet _ (print_timings);
                changed = tree_12x_iter(s, lam, delta);
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
