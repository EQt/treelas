/*
  Dynamic programming solver for line graphs
*/
#pragma once
#include <stdexcept>        // std::invalid_argument
#include <vector>
#include <cmath>            // std::abs

#include <graphidx/utils/timer.hpp>
#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/weights.hpp>

#include "event.hpp"
#include "clip.hpp"


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

    std::vector<Event> elem;
    Range pq {int(n), int(n-1)};
    std::vector<float_> ub;
    {
        Timer _ ("alloc");
        elem.resize(2*n);
        ub.reserve(n-1);
    }
    float_
        *lb = x,
        lam0 = float_(0.0);
    auto *el = elem.data();
    {
        Timer _ ("forward");
        DEBUG && printf("\n");
        for (size_t i = 0; i < n-1; i++) {
            lb[i] = clip<+1, check>(el, pq, +mu[i], -mu[i]*y[i] - lam0 + lam[i]);
            ub[i] = clip<-1, check>(el, pq, -mu[i], +mu[i]*y[i] - lam0 + lam[i]);
            lam0 = mu[i] > EPS ? lam[i] : std::min(lam0, lam[i]);
        }
    }
    {
        Timer _ ("backward");
        DEBUG && printf("\n");
        x[n-1] = clip<+1, check>(el, pq, mu[n-1], -mu[n-1]*y[n-1] - lam0 + 0);
        for (size_t i = n-1; i >= 1; i--)
            x[i-1] = clamp(x[i], lb[i-1], ub[i-1]);
    }
}
