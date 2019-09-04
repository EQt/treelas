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


template <typename E>
struct DeQue
{
private:
    std::vector<E> _e;
    size_t front = 1, back = 0;

public:
    /// allocate space for 2*n elements; resetting to the middle
    void reserve(const size_t n) { _e.reserve(2*n); front = n; back = n-1; }

    /// first or last element (depending on forward)
    template <bool forward = true>
    const E& peek() const { return forward ? _e[front] : _e[back]; }

    /// remove first/last element
    template <bool forward = true>
    E pop() { return forward ? _e[front++] : _e[back--]; }

    /// append an element to front/back of the queue
    template <bool forward = true>
    void push(E x) { if (forward) _e[--front] = x; else _e[++back] = x; }

    /// is the queue empty?
    operator bool() const { return front <= back; }
};


template<bool forward, bool need_check = false, typename float_ = double>
inline float_
clip(DeQue<Event> pq,
     float_ slope,
     float_ offset)
{
    const auto *e = &pq.peek<forward>();
    while (pq && slope * e->x + offset < 0) {
        offset += e->offset();
        slope += e->slope;
        pq.pop<forward>();
    }
    if (need_check && std::abs(slope) <= EPS) {
        return -1.0;
    }
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
    std::vector<float_> ub_;
    {
        Timer _ ("alloc");
        pq.reserve(n);
        ub_.reserve(n-1);
    }
    float_
        *ub = ub_.data(),
        *lb = x,
        lam0 = float_(0.0);

    for (size_t i = 0; i < n-1; i++) {
        lb[i] = clip<true>(pq, +mu[i], -mu[i] * y[i] - lam0 + lam[i]);
        ub[i] = clip<false>(pq, -mu[i], +mu[i] * y[i] - lam0 + lam[i]);
    }
}
