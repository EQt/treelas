/*
  Dynamic programming solver for tree graphs.
 */
#include "tree_dp.hpp"
#include <graphidx/bits/weights.hpp>


template <bool merge_sort, bool lazy_sort>
const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root)
{
    const Const<double> _lam (lam);
    const Const<double> _mu (mu);
    return tree_dp<merge_sort, lazy_sort, false>(n, x, y, parent,
                                                 _lam, _mu, root);
}


template <bool lazy_sort,
          bool merge_sort>
const double*
tree_dp_w(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root)
{
    if (parent[root] != root) {     // verify root property
        throw std::invalid_argument(std::string("parent[") +
                                    std::to_string(root) + "] = " +
                                    std::to_string(parent[root]) + " != root");
    }

    const Array<const double> _lam(lam);
    const Array<const double> _mu(mu);
    return tree_dp<merge_sort, lazy_sort, true>(n, x, y, parent,
                                                _lam, _mu, root);

    Timer::startit("free");
    return x;
}


// template instantiation
template
double const*
tree_dp_w<true, true>(
    size_t,
    double*,
    double const*,
    int const*,
    double const*,
    double const*,
    int);


template
double const*
tree_dp_w<false, true>(
    size_t,
    double*,
    double const*,
    int const*,
    double const*,
    double const*,
    int);


template
const double*
tree_dp<true, false>(    // tree_dp<true, true> does not make sense
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template
const double*
tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template
const double*
tree_dp<false, false>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);
