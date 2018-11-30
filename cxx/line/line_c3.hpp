/* Compared to cline.hpp, save more memory */
#pragma once


template <typename float_ = double>
void
dp_line_c3(const int n,
           const float_ *y,
           const float_ lam,
           float_ *beta);


extern template
void
dp_line_c3(const int n,
           const double *y,
           const double lam,
           double *beta);
