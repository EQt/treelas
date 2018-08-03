#pragma once

#define BLOCK_ALLOC 0
#define MALLOC      0


template <typename float_ = double>
void
dp_line_c(const int n,
          const float_ *y,
          const float_ lam,
          float_ *beta);
