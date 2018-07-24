#pragma once
#include <vector>
#include "postorder.hpp"


template <bool merge_sort>
const double*
dp_tree(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu = 1.0,
    const int root = 0);


extern template
const double*
dp_tree<true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


extern template
const double*
dp_tree<false>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


const double*
dp_tree_weighted(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root = 0);


double*
dp_dual(const int n,
        double *x,
        const int *parent,
        const int *post_ord = nullptr,
        double *alpha = nullptr,
        const int root = 0);


void
dp_gamma(const int n,
         double *gamma,
         const double *x,
         const double *alpha,
         const double *lam,
         const int *parent,
         const double root_val = 0.0);
