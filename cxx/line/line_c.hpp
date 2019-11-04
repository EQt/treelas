#pragma once
#include <cstddef>

template <typename float_ = double>
void
dp_line_c(const size_t n,
          const float_ *y,
          const float_ lam,
          float_ *beta);
