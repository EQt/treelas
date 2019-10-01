/*
  New implementation
*/
#pragma once
#include <stdexcept>        // std::invalid_argument
#include <vector>
#include <cmath>            // std::abs
#include <limits>

#include "utils/timer.hpp"
#include <graphidx/bits/weights.hpp>
#include <graphidx/std/deque.hpp>

#include "event.hpp"


static const auto EPS = 1e-10;
static const auto DEBUG = true;


template<typename float_ = float>
inline float_
clamp(float_ x, float_ a, float_ b)
{
    x = x > b ? b : x;
    x = x < a ? a : x; 
    return x;
}


template<bool forward, bool need_check = false, typename float_ = double>
inline float_
clip(DeQue<Event> &pq,
     float_ slope,
     float_ offset)
{
    #define dir (forward ? "f" : "b")
    if (DEBUG) {
        printf("clip_%s: (%+g, %+.2f)\n", dir, slope, offset);
        if (pq)
            printf(" test: %f\n", slope * pq.peek<forward>().x + offset);
    }
    while (pq && slope * pq.peek<forward>().x + offset < 0) {
        const Event e = pq.pop<forward>();
        offset += e.offset();
        slope += e.slope;
        DEBUG && printf(" lip_%s: (%+g, %+.2f)\n", dir, slope, offset);
    }
    if (need_check && std::abs(slope) <= EPS)
        return forward ?
            -std::numeric_limits<float_>::infinity() :
            +std::numeric_limits<float_>::infinity();
    const auto x = -offset/slope;
    DEBUG && printf("  ip_%s: (%+g, %+.2f)\n", dir, slope, offset);
    pq.push<forward>({x, slope});
    return x;
    #undef dir
}


template<typename float_, typename Wlam>
inline void
line_las(
    const size_t n,
    float_ *x,
    const float_ *y,
    const Wlam &lam)
{
    line_las(n, x, y, lam, ConstantWeights<float_>(float_(1.0)));
}


template<typename float_, typename Wlam, typename Wmu, bool check = true>
void
line_las(
    const size_t n,
    float_ *x,
    const float_ *y,
    const Wlam &lam,
    const Wmu &mu)
{
    if ((mu[n-1]) <= 0)
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
    {
        Timer _ ("forward");
        DEBUG && printf("\n");
        for (size_t i = 0; i < n-1; i++) {
            lb[i] = clip<true , check>(pq, +mu[i], -mu[i] * y[i] - lam0 + lam[i]);
            ub[i] = clip<false, check>(pq, -mu[i], +mu[i] * y[i] - lam0 + lam[i]);
            lam0 = mu[i] > EPS ? lam[i] : std::min(lam0, lam[i]);
        }
    }
    {
        Timer _ ("backward");
        DEBUG && printf("\n");
        x[n-1] = clip<true, check>(pq, mu[n-1], -mu[n-1] * y[n-1] - lam0 + 0);
        for (size_t i = n-1; i >= 1; i--)
            x[i-1] = clamp(x[i], lb[i-1], ub[i-1]);
    }
}
