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
    {   // Timer _ ("deriv init");
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
