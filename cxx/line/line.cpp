/**
  Implement the dynamic programming algorithm for line graphs.
 */
#include "line.hpp"

#include <vector>
#include <cmath>

#include <graphidx/bits/clamp.hpp>     // for clamp(x, lb, ub)
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/malloc.hpp>
#include <graphidx/utils/vecalloc.hpp>

#include "../clip.hpp"


template<typename float_, typename Event_>
void
dp_forward(
    const float_ *y,
    const float_ lam,
    float_ *lb,
    float_ *ub,
    Event_ *event,
    Range &pq,
    const size_t begin,
    const size_t end)
{
    const float_ mu = 1.0;
    { // i = 0
        const auto i = begin;
        lb[i] = clip_front(event, pq, mu, -mu*y[i] -0.0 + lam);
        ub[i] = clip_back (event, pq, mu, -mu*y[i] +0.0, +lam);
    }
    for (size_t i = 1; i < end-1; i++) {
        lb[i] = clip_front(event, pq, mu, -mu*y[i] -lam + lam);
        ub[i] = clip_back (event, pq, mu, -mu*y[i] +lam, +lam);
    }
}


template
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


template<typename float_, typename Event_>
void
dp_reverse(
    const float_ *y,
    const float_ lam,
    float_ *lb,
    float_ *ub,
    Event_ *event,
    Range &pq,
    const size_t begin,
    const size_t end)
{
    const float_ mu = 1.0;
    { // i = n-1
        const auto i = end-1;
        lb[i-1] = clip_front(event, pq, mu, -mu*y[i] -0.0 + lam);
        ub[i-1] = clip_back (event, pq, mu, -mu*y[i] +0.0, +lam);
    }
    for (size_t i = end-2; i > begin; i--) {
        lb[i-1] = clip_front(event, pq, mu, -mu*y[i] -lam + lam);
        ub[i-1] = clip_back (event, pq, mu, -mu*y[i] +lam, +lam);
    }
 }


template
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
