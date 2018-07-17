#pragma once
#include "event.hpp"
#include "queue.hpp"

#undef UNIQUE_PTR       // use "new T[n]" allocator?
#define MALLOC   1      // use Malloc(n) allocator?


template<typename float_ = double>
void
dp_line(
    const size_t n,
    float_ *x,
    const float_ *y,
    const float_ lam,
    const bool increasing = true);


extern template
void
dp_line(const size_t n,
        double *x,
        const double *y,
        const double lam,
        const bool increasing);
