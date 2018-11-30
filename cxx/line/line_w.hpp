#pragma once
#include "../event.hpp"
#include "../range.hpp"

/**
   Compute bounds lb and ub from y[n-1] to y[1].
   lb, ub need to have size n-1 (belong to the corresponding edges).
   Return optimal x[0].
*/
template<typename float_ = double,
         typename Event_ = Event2>
Range
dp_forward_w(
    const int n,
    float_ *lb,
    float_ *ub,
    const float_ *y,
    const float_ *mu,
    const float_ *lam,
    float_ &off,
    Event_ *event = nullptr);


// template is compiled somewhere else for those types
extern template
Range
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
Range
dp_reverse_w(
    const int n,
    float_ *lb,
    float_ *ub,
    const float_ *y,
    const float_ *mu,
    const float_ *lam,
    float_ &off,
    Event_ *event = nullptr);


extern template
Range
dp_reverse_w(
    const int n,
    double *lb,
    double *ub,
    const double *y,
    const double *mu,
    const double *lam,
    double &off,
    Event2 *event);


template<typename float_ = double>
void
line_w(
    const int n,
    float_ *x,
    const float_ *y,
    const float_ *mu,
    const float_ *lam);


extern template
void
line_w(
    const int n,
    double *x,
    const double *y,
    const double *mu,
    const double *lam);
