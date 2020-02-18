#pragma once
#include <cstddef>                      // for std::size_t
#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/minmax.hpp>
#include <graphidx/tree/postorder.hpp>
#include <graphidx/tree/bfs.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/perm.hpp>
#include <graphidx/utils/timer.hpp>

#undef DEBUG_ID
constexpr auto PRINT_MAX = 20;


template<typename float_ = float, typename int_ = int>
void
tree_apx(
    const size_t n,
    const int_ *parent,
    const float_ *y,
    const float_ lam,
    float_ *x,
    const int_ root_ = int_(-1),
    const int max_iter = 3,
    const bool print_timings = true,
    const bool reorder = true);


extern template
void
tree_apx(
    const size_t n,
    const int *parent,
    const float *y,
    const float lam,
    float *x,
    const int root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder);


extern template
void
tree_apx(
    const size_t n,
    const int *parent,
    const double *y,
    const double lam,
    double *x,
    const int root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder);


/**
   Data needed to perform an approximation iteration.

   Whether nodes `i` and `j = parent(i)` are in the same region is stored
   in the first bit of the `parent` array;
   you can query it by calling `same(i)`.
 */
template<typename float_ = float, typename int_ = int>
struct TreeApx
{
    const size_t n = 0;
    const bool is_linear;
#ifdef DEBUG_ID
    std::vector<int> id;
#endif
    float_ *y = nullptr;
    float_ *x = nullptr;
    float_ *deriv = nullptr;
    int_ *parent_ = nullptr;
    const int_ *porder;         // post-order (forward, i.e. upward)

    TreeApx(const size_t n, const int_ *porder, bool is_linear = true)
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
TreeApx<float_, int_>::iter(const float_ lam, const float_ delta)
{
    {   Timer _ ("deriv init");
        for (size_t i = 0; i < n; i++)
            deriv[i] = x[i] - y[i];
    }
    {   Timer _ (" forward");
        for (size_t i = 0; i < n-1; i++) {
            const auto v = is_linear ? i : porder[i];
#ifdef DEBUG_ID
            if (n <= PRINT_MAX)
                printf("\ni = %d: id = %d, v = %d, p = %d, p.id = %d",
                       int(i), int(id[v]), int(v), parent(v), id[parent(v)]);
#endif
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
        // if (n <= PRINT_MAX)
        //     printf(" root deriv = %+.3f xr = %+.3f x[root] = %+.3f\n",
        //            deriv[root], xr, x[root]);
        for (size_t i = n-1; i > 0; i--) {
            const auto v = is_linear ? i-1 : porder[i-1];
#ifdef DEBUG_ID
            if (n <= PRINT_MAX)
                printf("\ni = %d: id = %d, v = %d, p = %d, p.id = %d ",
                       int(i), int(id[v]), int(v), parent(v), id[parent(v)]);
#endif
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
                if (x[v] != x[p]) {
                    changed++;
                    divorce(v);
                }
                y[v] += x[v] < x[p] ? lam : float_(0.0);
                y[p] -= x[v] < x[p] ? lam : float_(0.0);
                y[v] -= x[v] > x[p] ? lam : float_(0.0);
                y[p] += x[v] > x[p] ? lam : float_(0.0);
            } else {
                x[v] += deriv[v] < 0 ? +delta : -delta;
            }
        }
    }

    return changed;
}



template<typename float_, typename int_>
void
tree_apx(
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
    Timer _ ("tree_apx:\n");

    std::vector<int_> porder;    // post order (forward)
    std::vector<int_> iorder;    // inverse of porder
    ChildrenIndex cidx;
    int_ root = root_;
    float_
        min_y = y[0],
        max_y = y[0];

    {
        Timer _ ("vector::resize");
        porder.reserve(n);
        iorder.reserve(n);
    }
    Timer tim ("TreeApx::alloc");
    TreeApx<float_, int_> s (n, porder.data(), reorder);
    tim.stop();
    if (root < 0) {
        Timer _ ("find_root");
        root = find_root(n, parent);
    }
    {
        Timer _ ("children idx");
        cidx.reset(n, parent, root);
    }

    if (true /*use_bfs*/)
    {
        Timer _ ("bfs");
        reversed_bfs_pi(porder.data(), cidx, s.parent_);
    } else {
        Timer _ ("dfs postorder\n");
        stack<int_> stack;
        post_order(porder.data(), cidx, stack);
    }
    {
        Timer _ ("parent bit");
        for (size_t i = 0; i < n; i++)
            s.parent_[i] |= decltype(s)::one;
    }
    if (porder[n-1] != root)
        throw std::runtime_error(
            std::string("tree_apx(): FATAL: ") +
            "forder[" + std::to_string(n-1) + "] = " +
            std::to_string(porder[n-1]) + " != " +
            std::to_string(root) + " = root");

    if (n <= PRINT_MAX) {
        Timer _ ("inverse order");
        invperm(n, iorder.data(), porder.data());
    }
    {   Timer _ ("init x,y");
#ifdef DEBUG_ID
        s.id.resize(n);
#endif
        if (reorder) {
            for (size_t i = 0; i < n; i++) {
                const auto ii = porder[i];
#ifdef DEBUG_ID
                s.id[i] = ii;
#endif
                s.y[i] = y[ii];
                min_y = std::min(s.y[i], min_y);
                max_y = std::max(s.y[i], max_y);
                // s.init_parent(i, iorder[parent[ii]]);
            }
        } else {
            for (size_t i = 0; i < n; i++) {
#ifdef DEBUG_ID
                s.id[i] = int(i);
#endif
                s.y[i] = y[i];
                min_y = std::min(s.y[i], min_y);
                max_y = std::max(s.y[i], max_y);
                s.init_parent(i, parent[i]);
            }
        }
        const float_ x0 = float_(0.5 * (min_y + max_y));
        for (size_t i = 0; i < n; i++)
            s.x[i] = x0;
    }
    if (n <= PRINT_MAX) {
        printf("   parent: ");
        print_int_list(Vec(parent, n));
        printf("postorder: ");
        print_int_list(Vec(porder.data(), n));
        printf("   iorder: ");
        print_int_list(Vec(iorder.data(), n));
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
        float_ delta = float_((max_y - min_y) * 0.5);
        for (int k = 0; k < max_iter; k++) {
            size_t changed = 0;
            Timer::log("%2ld ...%s", k+1, print_timings ? "\n" : "");

            delta *= float_(0.5);
            {
                TimerQuiet _ (print_timings);
                changed = s.iter(lam, delta);
#ifdef DEBUG_ID
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
#endif
            }
            if (changed)
                Timer::log("  %'d", changed);
            Timer::log("\n");
        }
    }

    {   Timer _ ("extract x");
        if (reorder) {
            for (size_t i = 0; i < n; i++)
                x[porder[i]] = s.x[i];
        } else {
            for (size_t i = 0; i < n; i++)
                x[i] = s.x[i];
        }
    }
}
