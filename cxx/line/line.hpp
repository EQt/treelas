#pragma once
#include "../event.hpp"
#include "../range.hpp"


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
