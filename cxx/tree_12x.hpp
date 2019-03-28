#pragma once
#include <cstddef>      // for std::size_t


template<typename float_ = float, typename int_ = int>
void
tree_12x(
    const size_t n,
    const int_ *parent,
    const float_ *y,
    const float_ lam,
    float_ *x,
    const int_ root_ = int_(-1),
    const int max_iter = 3,
    const bool print_timings = true,
    const bool reorder = false);


extern template
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


extern template
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
