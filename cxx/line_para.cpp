#include <iostream>
#include <vector>
#include <thread>

#include "utils/viostream.hpp"
#include "utils/timer.hpp"
#include "utils/minmax.hpp"         // for clip(x, lb, ub)
#include "utils/plotpwl.hpp"        // for debugging
#include "line_para.hpp"
#include "line.hpp"
#include "clip.hpp"
#include "queue.cpp"


template <typename float_>
void
line_para(const size_t n,
          const float_ *y,
          const float_ lam,
          float_ *x)
{
    std::vector<Event2> event_;
    std::vector<float_> ub_;
    {   Timer _ ("allocation");
        event_.reserve(2*n);
        ub_.reserve(n);
    }

    Event2 *event = event_.data();
    float_
        *ub = ub_.data(),
        *lb = x;

    const size_t n0 = n / 2;
    const size_t n1 = n - n0;

    Queue pq0 {int(0*n0 + n0), int(0*n0 + n0-1)};
    Queue pq1 {int(2*n0 + n1), int(2*n0 + n1-1)};

    {   Timer _ ("forward");
        std::thread t0 (dp_forward, y, lam, lb+0, ub+0, event, pq0, size_t(0), size_t(n0+1));
        std::thread t1 (dp_reverse, y, lam, lb+1, ub+1, event, pq1, n0, n);
        t0.join();
        t1.join();
    }

    Queue pq = merge(pq0, pq1, event);

    {   Timer _ ("root value");
        const float_ mu = 1.0;
        x[n0] = clip_back(event, pq, mu, -mu*y[n0] +2*lam, 0.0);
    }

    {   Timer _ ("backward");
        for (size_t i = n0+1; i < n; i++)
            x[i] = clip(x[i-1], lb[i], ub[i]);
        for (int i = int(n0-1); i >= 0; i--)
            x[i] = clip(x[i+1], lb[i], ub[i]);
    }
}


template
void
line_para(const size_t n,
          const double *y,
          const double lam,
          double *x);


#ifndef PARALLEL
#  define PARALLEL 0
#endif
#if PARALLEL

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
