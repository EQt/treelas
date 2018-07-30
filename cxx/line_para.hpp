/**
   Parallel implementation.
*/
#pragma once

template <typename float_ = double>
void
line_para(const int n,
          const float_ *y,
          const float_ lam,
          float_ *x);


extern template
void
line_para(const int n,
          const double *y,
          const double lam,
          double *x);
