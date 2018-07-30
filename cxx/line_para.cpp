#include <vector>

#include "utils/timer.hpp"
#include "line_para.hpp"
#include "line.hpp"


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
