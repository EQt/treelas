/*
  Dynamic programming solver for line graphs
*/
#pragma once
#include <cmath>            // std::abs
#include <stdexcept>        // std::invalid_argument
#include <vector>

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/weights.hpp>
#include <graphidx/utils/timer.hpp>

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
    line_las(n, x, y, lam, UnitWeights<float_>());
}


template<typename float_, typename Wlam>
std::vector<float_>
line_las(
    const size_t n,
    const float_ *y,
    const Wlam &lam)
{
    std::vector<float_> x;
    x.resize(n);
    line_las(n, x.data(), y, lam, UnitWeights<float_>());
    return x;
}


template<typename float_, typename Wlam>
std::vector<float_>
line_las(
    const std::vector<float_> &y,
    const Wlam &lam)
{

    return line_las(y.size(), y.data(), lam);
}


template<typename float_, typename Wlam, typename Wmu, bool check = true>
void
line_las(
    const size_t n,
    float_ *x,
    const float_ *y,
    const Wlam &lam,
    const Wmu &mu = UnitWeights<float_>())
{
    if ((mu[n-1]) <= 0)
        throw std::invalid_argument("End node must not be latent");

    std::vector<Event> elem;
    std::vector<float_> ub;
    Range pq {int(n), int(n-1)};
    {
        Timer _ ("alloc");
        elem.resize(2*n);
        ub.reserve(n-1);
    }
    auto *el = elem.data();
    float_
        *lb = x,
        lam0 = float_(0.0);
    {
        Timer _ ("forward");
        DEBUG && printf("\n");
        for (size_t i = 0; i < n-1; i++) {
            lb[i] = clip<+1, check>(el, pq, +mu[i], -mu[i]*y[i] - lam0 + lam[i]);
            ub[i] = clip<-1, check>(el, pq, -mu[i], +mu[i]*y[i] - lam0 + lam[i]);
            lam0 = (check && mu[i] > EPS) ? lam[i] : std::min(lam0, lam[i]);
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
