/**
  Implement the dynamic programming algorithm for line graphs.
 */
#include "line.hpp"

#include <vector>
#include <cmath>
#include <memory>               // for std::unique_ptr
#include <cstdlib>              // for malloc

#include "utils/timer.hpp"
#include "utils/malloc.hpp"
#include "utils/vecalloc.hpp"
#include "bits/minmax.hpp"     // for clip(x, lb, ub)

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


template<typename float_>
void
line_dp(const size_t n,
        float_ *x,
        const float_ *y,
        const float_ lam,
        const bool increasing)
{
    const float_ mu = 1.0;

    Timer t ("alloc");
#ifdef UNIQUE_PTR
    std::unique_ptr<Event> event_ (new Event[2*n]);
    std::unique_ptr<float_> ub_ (new float_[2*n]);
    Event *event = event_.get();
    float_ *ub = ub_.get();
#elif defined MALLOC
    Malloc<Event> event_(2*n);
    Malloc<float_> ub_ (n);
    Event *event = event_.data();
    float_ *ub = ub_.data();
#else
    std::vector<Event> event_;
    std::vector<float_> ub_;
    event_.reserve(2*n);
    ub_.reserve(n-1);

    Event *event = event_.data();
    float_ *ub = ub_.data();
#endif
    t.stop();

    Range pq {int(n), int(n-1)};

    if (increasing) {
        float_ *lb = x;
        {   Timer _ ("forward");
            dp_forward(y, lam, lb, ub, event, pq, 0, n);
        }

        {   Timer _ ("backward");
            x[n-1] = clip_front(event, pq, mu, -mu*y[n-1] -lam + 0.0);
            for (int i = int(n-2); i >= 0; i--)
                x[i] = clip(x[i+1], lb[i], ub[i]);
        }
    } else {
        float_ *lb = x+1;
        {   Timer _ ("reverse");
            dp_reverse(y, lam, lb, ub, event, pq, 0, n);
        }

        {   Timer _ ("backward");
            x[0] = clip_front(event, pq, mu, -mu*y[0] -lam + 0.0);
            for (size_t i = 1; i < n; i++)
                x[i] = clip(x[i-1], lb[i-1], ub[i-1]);
        }
    }
}


template
void
line_dp(const size_t n,
        double *x,
        const double *y,
        const double lam,
        const bool);
