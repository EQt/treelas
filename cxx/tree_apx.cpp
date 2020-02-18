#include "tree_apx.hpp"


template
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


template
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
