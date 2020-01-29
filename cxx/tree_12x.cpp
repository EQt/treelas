#include "tree_12x.hpp"

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/minmax.hpp>
#include <graphidx/tree/postorder.hpp>
#include <graphidx/tree/bfs.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/perm.hpp>
#include <graphidx/utils/timer.hpp>

#define PRINT_MAX 20


template<typename float_ = float, typename int_ = int>
struct TreeApx
{
    const size_t n = 0;
    const bool is_linear;
    std::vector<int> id;
    float_ *y = nullptr;
    float_ *x = nullptr;
    float_ *deriv = nullptr;
    int_ *parent_ = nullptr;
    const int_ *porder;         // post-order (forward, i.e. upward)

    TreeApx(const size_t n, const int_ *porder, bool is_linear)
        : n(n), is_linear(is_linear), porder(porder) {
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

    ~TreeApx() {
        if (y) delete[] y;
        if (x) delete[] x;
        if (deriv) delete[] deriv;
        if (parent_) delete[] parent_;
    }

    size_t iter(const float_ lam, const float_ delta);
};


template<typename float_, typename int_>
size_t
TreeApx<float_, int_>::iter(
    const float_ lam,
    const float_ delta)
{
    {   // Timer _ ("deriv init");
        for (size_t i = 0; i < n; i++)
            deriv[i] = x[i] - y[i];
    }

    {   Timer _ (" forward");
        for (size_t i = 0; i < n-1; i++) {
            const auto v = is_linear ? i : porder[i];
            if (n <= PRINT_MAX)
                printf("\ni = %d: id = %d, v = %d, p = %d, p.id = %d",
                       int(i), int(id[v]), int(v), parent(v), id[parent(v)]);
            if (same(v)) {
                const auto p = parent(v);
                deriv[p] += clamp(deriv[v], -lam, +lam);
            }
        }
    }

    size_t changed = 0;
    {   Timer _ (" backward");
        const auto root = is_linear ? n-1 : porder[n-1];
        const auto xr = deriv[root] > 0 ? -delta : +delta;
        x[root] += xr;
        if (n <= PRINT_MAX)
            printf(" root deriv = %+.3f xr = %+.3f x[root] = %+.3f\n",
                   deriv[root], xr, x[root]);

        for (size_t i = n-1; i > 0; i--) {
            const auto v = is_linear ? i-1 : porder[i-1];
            // printf("\ni = %d: id = %d, v = %d, p = %d, p.id = %d ",
            //        int(i), int(id[v]), int(v), parent(v), id[parent(v)]);

            if (same(v)) {
                // printf(" deriv = %+.3f", deriv[v]);

                if (deriv[v] > lam) {
                    x[v] -= delta;
                } else if (deriv[v] < -lam) {
                    x[v] += delta;
                } else {
                    x[v] = x[parent(v)];
                    // printf(" x = %+.3f parent", x[v]);
                    continue;
                }

                // printf(" x = %+.3f", x[v]);

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

    std::vector<int_> porder;    // post order (forward)
    std::vector<int_> iorder;    // inverse of porder
    ChildrenIndex cidx;
    int_ root = root_;

    Timer::log("lam = %f\n", lam);

    if (root < 0) {
        Timer _ ("find_root");
        root = find_root(n, parent);
    }
    {
        Timer _ ("vector::resize");
        porder.resize(n);
        iorder.resize(n);
    }
    {
        Timer _ ("children idx");
        cidx.reset(n, parent, root);
    }
    //if (use_bfs)
    {
        Timer _ ("bfs");
        reversed_bfs(porder, cidx);
    } /*else {
        Timer _ ("dfs postorder\n");
        stack<int_> stack;
        post_order(porder.data(), cidx, stack);
       }*/
    if (porder[n-1] != root)
        throw std::runtime_error(
            std::string("tree_12x(): FATAL: ") +
            "forder[" + std::to_string(n-1) + "] = " +
            std::to_string(porder[n-1]) + " != " +
            std::to_string(root) + " = root");

    {   Timer _ ("inverse order");
        invperm(iorder, porder);
    }

    Timer tim ("alloc");
    TreeApx<float_, int_> s (n, porder.data(), reorder);
    tim.stop();

    float_ min_y, max_y, delta;
    {   Timer _ ("minmax y");
        find_minmax(y, n, min_y, max_y);
        delta = float_((max_y - min_y) * 0.5);
    }

    if (n <= PRINT_MAX) {
        printf("   parent: ");
        print_int_list(Vec(parent, n));
        printf("postorder: ");
        print_int_list(porder);
        printf("   iorder: ");
        print_int_list(iorder);
    }

    {   Timer _ ("init x,y,parent");
        s.id.resize(n);
        const float_ x0 = float_(0.5 * (min_y + max_y));
        for (size_t i = 0; i < n; i++)
            s.x[i] = x0;
        if (reorder) {
            for (size_t i = 0; i < n; i++) {
                const auto ii = porder[i];
                s.id[i] = ii;
                s.y[i] = y[ii];
                s.init_parent(i, iorder[parent[ii]]);
            }
        } else {
            for (size_t i = 0; i < n; i++) {
                s.id[i] = int(i);
                s.y[i] = y[i];
                s.init_parent(i, parent[i]);
            }
        }
    }
    if (n <= PRINT_MAX) {
        printf("deriv: [");
        for (size_t i = 0; i < n; i++)
            printf("%.3f ", s.deriv[reorder ? iorder[i] : i]);
        printf("]\n");
        printf("    x: [");
        for (size_t i = 0; i < n; i++)
            printf("%.3f ", s.x[reorder ? iorder[i] : i]);
        printf("]\n");
    }
    {   Timer _ ("iterations:\n");
        for (int k = 0; k < max_iter; k++) {
            size_t changed = 0;
            Timer::log("%2ld ...%s", k+1, print_timings ? "\n" : "");

            delta = float_(0.5*delta);
            {
                TimerQuiet _ (print_timings);
                changed = s.iter(lam, delta);
                if (n <= PRINT_MAX) {
                    printf("deriv: [");
                    for (size_t i = 0; i < n; i++)
                        printf("%.3f ", s.deriv[reorder ? iorder[i] : i]);
                    printf("]\n");
                    printf("    x: [");
                    for (size_t i = 0; i < n; i++)
                        printf("%.3f ", s.x[reorder ? iorder[i] : i]);
                    printf("]\n");
                }
            }
            if (changed)
                Timer::log("  %'d", changed);
            Timer::log("\n");
        }
    }

    {   Timer _ ("extract x");
        if (reorder) {
            for (size_t i = 0; i < n; i++) {
                x[i] = s.x[iorder[i]];
            }
        } else {
            for (size_t i = 0; i < n; i++)
                x[i] = s.x[i];
        }
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
