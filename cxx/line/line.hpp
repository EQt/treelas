#pragma once
#include "../event.hpp"
#include "../range.hpp"

#undef UNIQUE_PTR       // use "new T[n]" allocator?
#undef MALLOC           // use Malloc(n) allocator?


template<typename float_ = double>
void
line_dp(
    const size_t n,
    float_ *x,
    const float_ *y,
    const float_ lam,
    const bool increasing = true);


extern template
void
line_dp(const size_t n,
        double *x,
        const double *y,
        const double lam,
        const bool increasing);


template<typename float_ = double, typename Event_ = Event>
void
dp_forward(
    const float_ *y,
    const float_ lam,
    float_ *lb,
    float_ *ub,
    Event_ *event,
    Range &pq,
    const size_t begin,
    const size_t end);


extern template
void
dp_forward(
    const double *y,
    const double lam,
    double *lb,
    double *ub,
    Event *event,
    Range &pq,
    const size_t begin,
    const size_t end);


template<typename float_ = double, typename Event_ = Event>
void
dp_reverse(
    const float_ *y,
    const float_ lam,
    float_ *lb,
    float_ *ub,
    Event_ *event,
    Range &pq,
    const size_t begin,
    const size_t end);


extern template
void
dp_reverse(
    const double *y,
    const double lam,
    double *lb,
    double *ub,
    Event *event,
    Range &pq,
    const size_t begin,
    const size_t end);
