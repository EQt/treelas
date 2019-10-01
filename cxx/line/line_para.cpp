#include "line_para.hpp"

#include <iostream>
#include <vector>
#include <thread>

#include <graphidx/bits/minmax.hpp>          // for clip(x, lb, ub)
#include <graphidx/utils/timer.hpp>

#include "../clip.hpp"
#include "../merge.hpp"
#include "line.hpp"                     // for dp_forward(...)


template <typename float_>
void
line_para(const size_t n,
          const float_ *y,
          const float_ lam,
          float_ *x,
          const bool parallel)
{
    std::vector<Event> event_;
    std::vector<float_> ub_;
    {   Timer _ ("allocation");
        event_.reserve(2*n);
        ub_.reserve(n);
    }

    Event *event = event_.data();
    float_
        *ub = ub_.data(),
        *lb = x;

    const size_t n0 = n / 2;
    const size_t n1 = n - n0;

    Range pq0 {int(0*n0 + n0), int(0*n0 + n0-1)};
    Range pq1 {int(2*n0 + n1), int(2*n0 + n1-1)};

    {   Timer _ ("forward");
        if (parallel) {
            std::thread t0 ([&]() {
                    dp_forward(y, lam, lb+0, ub+0, event, pq0,  0, n0+1);
                });
            std::thread t1 ([&]() {
                    dp_reverse(y, lam, lb+1, ub+1, event, pq1, n0, n);
                });
            t0.join();
            t1.join();
        } else {
            dp_forward(y, lam, lb+0, ub+0, event, pq0,  0, n0+1);
            dp_reverse(y, lam, lb+1, ub+1, event, pq1, n0, n);
        }
    }

    Range pq = merge2(pq0, pq1, event);

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
          double *x,
          const bool parallel);
