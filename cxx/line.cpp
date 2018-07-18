/**
  Implement the dynamic programming algorithm for line graphs.
 */
#include <vector>
#include <cmath>
#include <memory>               // for std::unique_ptr
#include <cstdlib>              // for malloc

#include "utils/timer.hpp"
#include "utils/malloc.hpp"
#include "utils/vecalloc.hpp"
#include "utils/minmax.hpp"

#include "line.hpp"
#include "clip.hpp"



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
