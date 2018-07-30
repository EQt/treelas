#include "line_para.hpp"
#include "line.hpp"

template <typename float_>
void
line_para(const int n,
          const float_ *y,
          const float_ lam,
          float_ *beta)
{
    const int n0 = n / 2;
    const int n1 = n - n0;
}


template
void
line_para(const int n,
          const double *y,
          const double lam,
          double *beta);




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
