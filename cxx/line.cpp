/*
  Implement the dynamic programming algorithm for line graphs.
  TODO: Make clip_front, clip_back also templates for generic float_ type
*/
#include <vector>
#include <cmath>
#include <memory>               // for std::unique_ptr
#include <cstdlib>              // for malloc

#include "utils/timer.hpp"
#include "utils/malloc.hpp"
#include "utils/vecalloc.hpp"

#include "line.hpp"
#include "clip.hpp"
#include "clip2.cpp"


template<typename float_, typename Event_>
Queue
dp_forward_w(
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
    for (int i = n-1; i > 0; i--) {
        const auto lami = lam[i-1];
        lb[i-1] = clip_fronw(event, pq, mu[i], -mu[i]*y[i] -off, -lami, min_y);
        ub[i-1] = clip_backw(event, pq, mu[i], -mu[i]*y[i] +off, +lami, max_y);
        off = mu[i] > 1e-10 ? lami : std::min(lami, off);
    }
    return pq;
}


template<typename float_, typename Event_>
float_
dp_reverse(
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



template<typename float_>
void
dp_line_w(
    const int n,
    float_ *x,
    const float_ *y,
    const float_ *mu,
    const float_ *lam)
{
    std::vector<float_>
        lb_ (n-1), ub_ (n-1);
    float_
        *lb = lb_.data(),
        *ub = ub_.data();

    Queue pq = dp_forward_w(n, lb, ub, y, mu, lam);
    x[0] = clip_front(event, pq, mu[0], -mu[0]*y[0] -off, 0.0);
    for (int i = 1; i < n; i++) {
        x[i] = clip(x[i-1], lb[i-1], ub[i-1]);
    }
}


// template instantiation for float_ = double
template double
dp_forward(
    const int n,
    double *lb,
    double *ub,
    const double *y,
    const double *mu,
    const double *lam,
    Event2 *event = nullptr);


template double
dp_reverse(
    const int n,
    double *lb,
    double *ub,
    const double *y,
    const double *mu,
    const double *lam,
    Event2 *event = nullptr);


template
void
dp_line_w(
    const int n,
    double *x,
    const double *y,
    const double *mu,
    const double *lam);


template<typename float_>
void
dp_line(const size_t n,
        float_ *x,
        const float_ *y,
        const float_ lam,
        const bool increasing)
{
    const float_ mu = 1.0;

    Timer t ("alloc");
#ifdef UNIQUE_PTR
    std::unique_ptr<Event2> event_ (new Event2[2*n]);
    std::unique_ptr<float_> ub_ (new float_[2*n]);
    Event2 *event = event_.get();
    float_ *ub = ub_.get();
#elif defined MALLOC
    Malloc<Event2> event_(2*n);
    Malloc<float_> ub_ (n);
    Event2 *event = event_.data();
    float_ *ub = ub_.data();
#else
    std::vector<Event2> event_ (2*n);
    std::vector<float_> ub_ (n-1);
    Event2 *event = event_.data();
    float_ *ub = ub_.data();
#endif
    t.stop();

    Queue pq {int(n), int(n-1)};

    if (increasing) {
        float_ *lb = x;
        {   Timer _ ("forward");
            { // i = n-1
                const auto i = 0;
                lb[i] = clip_front(event, pq, mu, -mu*y[i] -0.0, -lam);
                ub[i] = clip_back (event, pq, mu, -mu*y[i] +0.0, +lam);
            }
            for (size_t i = 1; i < n-1; i++) {
                lb[i] = clip_front(event, pq, mu, -mu*y[i] -lam, -lam);
                ub[i] = clip_back (event, pq, mu, -mu*y[i] +lam, +lam);
            }
        }

        {   Timer _ ("backward");
            x[n-1] = clip_front(event, pq, mu, -mu*y[n-1] -lam, 0.0);
            for (int i = int(n-2); i >= 0; i--) {
                x[i] = clip(x[i+1], lb[i], ub[i]);
            }
        }
    } else {
        float_ *lb = x+1;
        {   Timer _ ("forward");
            { // i = n-1
                const auto i = n-1;
                lb[i-1] = clip_front(event, pq, mu, -mu*y[i] -0.0, -lam);
                ub[i-1] = clip_back (event, pq, mu, -mu*y[i] +0.0, +lam);
            }
            for (size_t i = n-2; i > 0; i--) {
                lb[i-1] = clip_front(event, pq, mu, -mu*y[i] -lam, -lam);
                ub[i-1] = clip_back (event, pq, mu, -mu*y[i] +lam, +lam);
            }
        }

        {   Timer _ ("backward");
            x[0] = clip_front(event, pq, mu, -mu*y[0] -lam, 0.0);
            for (size_t i = 1; i < n; i++) {
                x[i] = clip(x[i-1], lb[i-1], ub[i-1]);
            }
        }
    }
}


template
void
dp_line(const size_t n,
        double *x,
        const double *y,
        const double lam,
        const bool);



#ifndef PARALLEL
#  define PARALLEL 0
#endif
#if PARALLEL

#include <thread>

// https://github.com/uchicago-cs/cmsc12300/tree/master/examples/cpp/concurrency/simple

void
dp_line_par()
{
    const int n0 = n / 2;
    const int n1 = n - n0;
    std::thread t0 (dp_forward, n0, lb, ub);
    std::thread t1 (dp_forward, n1, lb, ub);

    t0.join();
    t1.join();
}

#endif
