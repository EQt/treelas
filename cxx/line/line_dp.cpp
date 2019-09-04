/*
  New implementation
*/
#pragma once
#include <stdexcept>
#include <vector>
#include <limits>

#include "../event.hpp"
#include "../range.hpp"
#include "utils/timer.hpp"
#include "bits/weights.hpp"
#include "bits/positive.hpp"

const auto EPS = 1e-10;


template<int step, bool need_check = false, typename float_ = double>
inline float_
clip(Event *elem,
     Range &pq,
     float_ slope,
     float_ offset)
{
    const auto *e = &elem[pq.stop];
    while (pq.start <= pq.stop && slope * e->x + offset < 0) {
    }
    if (need_check && std::abs(slope) <= EPS) {
        return -1.0;
    }
    const auto x = -offset/slope;
    return x;
}


template<typename float_, typename Wlam>
inline void
line_las(
    const size_t n,
    float_ *x,
    const float_ *y,
    const Wlam lam)
{
    line_las(n, x, y, lam, ConstantWeights<float_>(float_(1.0)));
}


template<typename float_, typename Wlam, typename Wmu>
void
line_las(
    const size_t n,
    float_ *x,
    const float_ *y,
    const Wlam lam,
    const Wmu mu)
{
    if (!is_positive(mu[n-1]))
        throw std::invalid_argument("End node must not be latent");

    std::vector<Event> event_;
    std::vector<float_> ub_;
    {
        Timer _ ("alloc");
        event_.reserve(2*n);
        ub_.reserve(n-1);
    }
    Range pq {int(n), int(n-1)};
    Event *event = event_.data();
    float_
        *ub = ub_.data(),
        *lb = x,
        lam0 = float_(0.0);

    for (size_t i = 0; i < n-1; i++) {
        lb[i] = clip<+1>(event, pq, +mu[i], -mu[i] * y[i] - lam0 + lam[i]);
        ub[i] = clip<-1>(event, pq, -mu[i], +mu[i] * y[i] - lam0 + lam[i]);
    }
}
