#pragma once

#undef BLOCK_ALLOC
#undef MALLOC


template <typename float_ = double>
void
dp_line_c(const int n,
          const float_ *y,
          const float_ lam,
          float_ *beta);
