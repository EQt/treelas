/**
   Parallel implementation.
*/
#pragma once
#ifndef PARALLEL
#  define PARALLEL 1
#endif


template <typename float_ = double>
void
line_para(const size_t n,
          const float_ *y,
          const float_ lam,
          float_ *x);


extern template
void
line_para(const size_t n,
          const double *y,
          const double lam,
          double *x);
