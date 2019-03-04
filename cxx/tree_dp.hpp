#pragma once


template <bool merge_sort, bool lazy_sort = false>
const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu = 1.0,
    const int root = 0);


extern template
const double*
tree_dp<true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


extern template
const double*
tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


extern template
const double*
tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template <bool lazy_sort>
const double*
tree_dp_w(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root = 0);


extern template
const double*
tree_dp_w<true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root);


extern template
const double*
tree_dp_w<false>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root);
