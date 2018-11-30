/**
   Parallel implementation.
*/

template <typename float_ = double>
void
line_para(const size_t n,
          const float_ *y,
          const float_ lam,
          float_ *x,
          const bool parallel = false);


extern template
void
line_para(const size_t n,
          const double *y,
          const double lam,
          double *x,
          const bool parallel);
