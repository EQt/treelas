/**
  Implement the dynamic programming algorithm for *weighted* line graphs.
 */
#include <vector>
#include <cmath>
#include <memory>               // for std::unique_ptr
#include <cstdlib>              // for malloc

#include "utils/timer.hpp"
#include "utils/malloc.hpp"
#include "utils/vecalloc.hpp"
#include "utils/minmax.hpp"

#include "line.hpp"
#include "clip.hpp"


template<typename float_, typename Event_>
Queue
dp_forward_w(
    const int n,
    float_ *lb,
    float_ *ub,
    const float_ *y,
    const float_ *mu,
    const float_ *lam,
    float_ &off,
    Event_ *event)
{
    VecAlloc<Event_> _ (&event, size_t(2*n));
    double min_y, max_y;
    find_minmax(y, n, min_y, max_y);
    Queue pq {n, n-1};
    off = 0;
    for (int i = n-1; i > 0; i--) {
        const auto lami = lam[i-1];
        lb[i-1] = clip_fronw(event, pq, mu[i], -mu[i]*y[i] -off, -lami, min_y);
        ub[i-1] = clip_backw(event, pq, mu[i], -mu[i]*y[i] +off, +lami, max_y);
        off = mu[i] > 1e-10 ? lami : std::min(lami, off);
    }
    return pq;
}


// template instantiation for float_ = double
template
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


template<typename float_, typename Event_>
float_
dp_reverse_w(
    const int n,
    float_ *lb,
    float_ *ub,
    const float_ *y,
    const float_ *mu,
    const float_ *lam,
    Event_ *event)
{
    VecAlloc<Event_> _ (&event, size_t(2*n));
    double min_y, max_y;
    find_minmax(y, n, min_y, max_y);
    Queue pq {n, n-1};
    double off = 0;
    for (int i = 0; i < n-1; i++) {
        const auto lami = lam[i];
        lb[i] = clip_fronw(event, pq, mu[i], -mu[i]*y[i] -off, -lami, min_y);
        ub[i] = clip_backw(event, pq, mu[i], -mu[i]*y[i] +off, +lami, max_y);
        off = mu[i] > 1e-10 ? lami : std::min(lami, off);
    }
    return clip_front(event, pq, mu[n-1], -mu[n-1]*y[n-1] -off, 0.0);
}


template double
dp_reverse_w(
    const int n,
    double *lb,
    double *ub,
    const double *y,
    const double *mu,
    const double *lam,
    Event2 *event = nullptr);


template<typename float_>
void
line_w(
    const int n,
    float_ *x,
    const float_ *y,
    const float_ *mu,
    const float_ *lam)
{
    std::vector<float_>
        lb_ (n-1), ub_ (n-1);
    std::vector<Event2> events_ (2*n);
    float_
        *lb = lb_.data(),
        *ub = ub_.data();
    Event2 *event = events_.data();

    double off;
    Queue pq = dp_forward_w(n, lb, ub, y, mu, lam, off, event);
    x[0] = clip_front(event, pq, mu[0], -mu[0]*y[0] -off, 0.0);
    for (int i = 1; i < n; i++) {
        x[i] = clip(x[i-1], lb[i-1], ub[i-1]);
    }
}


template
void
line_w(
    const int n,
    double *x,
    const double *y,
    const double *mu,
    const double *lam);
