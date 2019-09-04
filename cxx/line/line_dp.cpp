/*
  New implementation
*/
#pragma once
#include <stdexcept>
#include <vector>
#include <limits>

#include "../event.hpp"
#include "../clip.hpp"
#include "utils/timer.hpp"
#include "bits/weights.hpp"
#include "bits/positive.hpp"
#include "deque.hpp"

const auto EPS = 1e-10;


template<typename float_ = float>
inline float_
clap(float_ x, float_ a, float_ b)
{
    return x >= b ? b : (x <= a ? a : x);
}



template<bool forward, bool need_check = false, typename float_ = double>
inline float_
clip(DeQue<Event> &pq,
     float_ slope,
     float_ offset)
{
    const auto *e = &pq.peek<forward>();
    while (pq && slope * e->x + offset < 0) {
        offset += e->offset();
        slope += e->slope;
        pq.pop<forward>();
    }
    if (need_check && std::abs(slope) <= EPS)
        return forward ?
            -std::numeric_limits<float_>::infinity() :
            +std::numeric_limits<float_>::infinity();
    const auto x = -offset/slope;
    pq.push<forward>({x, slope});
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

    DeQue<Event> pq;
    std::vector<float_> ub;
    {
        Timer _ ("alloc");
        pq.reserve(n);
        ub.reserve(n-1);
    }
    float_
        *lb = x,
        lam0 = float_(0.0);

    for (size_t i = 0; i < n-1; i++) {
        lb[i] = clip<true>(pq, +mu[i], -mu[i] * y[i] - lam0 + lam[i]);
        ub[i] = clip<false>(pq, -mu[i], +mu[i] * y[i] - lam0 + lam[i]);
    }

    x[n-1] = clip<true>(pq, mu[n-1], -mu[n-1] * y[n-1] - lam0 + 0.0);
    for (size_t i = n-1; i >= 1; i--)
        x[i-1] = clap(x[i], lb[i-1], ub[i-1]);
}