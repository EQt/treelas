#pragma once


double*
tree_dual(const int n,
          double *x,
          const int *parent,
          const int *_postord,
          double *alpha,
          const int root);


void
tree_dual_gap(
    const int n,
    double *gamma,
    const double *x,
    const double *alpha,
    const double *lam,
    const int *parent,
    const double root_val = 0.0);
