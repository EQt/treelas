#pragma once
#include "event.hpp"
#include "queue.hpp"

#undef UNIQUE_PTR       // use "new T[n]" allocator?
#define MALLOC   1      // use Malloc(n) allocator?


/**
   Compute bounds lb and ub from y[n-1] to y[1].
   lb, ub need to have size n-1 (belong to the corresponding edges).
   Return optimal x[0].
*/
template<typename float_ = double,
         typename Event_ = Event2>
Queue
dp_forward_w(
    const int n,
    float_ *lb,
    float_ *ub,
    const float_ *y,
    const float_ *mu,
    const float_ *lam,
    double &off,
    Event_ *event = nullptr);


// template is compiled somewhere else for those types
extern template
Queue
dp_forward_w(
    const int n,
    double *lb,
    double *ub,
    const double *y,
    const double *mu,
    const double *lam,
    double &off,
    Event2 *event);


/**
   Compute bounds lb and ub from y[0] to y[n-2].
   lb, ub need to have size n-1 (belong to the corresponding edges).
   Return optimal x[n-1].
*/
template<typename float_ = double,
         typename Event_ = Event2>
float_
dp_reverse(
    const int n,
    float_ *lb,
    float_ *ub,
    const float_ *y,
    const float_ *mu,
    const float_ *lam,
    Event_ *event = nullptr);


extern template
double
dp_reverse(
    const int n,
    double *lb,
    double *ub,
    const double *y,
    const double *mu,
    const double *lam,
    Event2 *event);


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


template<typename float_ = double>
void
dp_line_w(
    const int n,
    float_ *x,
    const float_ *y,
    const float_ *mu,
    const float_ *lam);


extern template
void
dp_line_w(
    const int n,
    double *x,
    const double *y,
    const double *mu,
    const double *lam);
