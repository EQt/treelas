#include <vector>

#include "utils/timer.hpp"
#include "utils/minmax.hpp"         // for clip(x, lb, ub)
#include "line_para.hpp"
#include "line.hpp"
#include "clip.hpp"
#include "queue.cpp"


template <typename float_>
void
line_para(const int n,
          const float_ *y,
          const float_ lam,
          float_ *x)
{
    std::vector<Event2> event_;
    std::vector<float_> ub_;
    {   Timer _ ("allocation");
        event_.reserve(2*n);
        ub_.reserve(n-1);
    }

    Event2 *event = event_.data();
    float_
        *ub = ub_.data(),
        *lb = x;

    const int n0 = n / 2;
    const int n1 = n - n0;

    Queue pq0 {int(0*n0 + n0), int(0*n0 + n0-1)};
    Queue pq1 {int(2*n0 + n1), int(2*n0 + n1-1)};

    {   Timer _ ("forward halve");
        dp_forward(y, lam, lb, ub, event, pq0, 0, n0);
    }
    {   Timer _ ("reverse halve");
        dp_reverse(y, lam, lb, ub, event, pq1, n0, n);
    }

    merge(pq0, pq1, event);

    Queue &pq = pq0;
    {   Timer _ ("backward");
        const float_ mu = 1.0;
        x[n0-1] = clip_front(event, pq, mu, -mu*y[n0-1] -lam, 0.0);
        for (int i = int(n-2); i >= 0; i--) {
            x[i] = clip(x[i+1], lb[i], ub[i]);
        }
    }
}


template
void
line_para(const int n,
          const double *y,
          const double lam,
          double *x);


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
