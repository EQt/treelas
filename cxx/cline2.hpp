/* Compared to cline.hpp, save more memory */
#pragma once

#define BLOCK_ALLOC 1
#define MALLOC      1


template <typename float_ = double>
void
dp_line_c2(const int n,
           const float_ *y,
           const float_ lam,
           float_ *beta);


extern template
void
dp_line_c2(const int n,
           const double *y,
           const double lam,
           double *beta);
