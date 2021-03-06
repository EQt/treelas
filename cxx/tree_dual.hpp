#pragma once
#include <cstddef>              // std::size_t


double*
tree_dual(const size_t n,
          double *x,
          const int *parent,
          const int *_postord,
          double *alpha,
          const int root,
          const bool tree_orientation = true);


void
tree_dual_gap(
    const size_t n,
    double *gamma,
    const double *x,
    const double *alpha,
    const double *lam,
    const int *parent,
    const double root_val = 0.0);


void
tree_dual_gap(
    const size_t n,
    double *gamma,
    const double *x,
    const double *alpha,
    const double lam,
    const int *parent,
    const double root_val = 0.0);
