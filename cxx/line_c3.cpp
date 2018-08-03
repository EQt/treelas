/**
   Aim at being more cache efficient than others
*/
#include "line_c3.hpp"

#include <vector>
#include <stdexcept>

#ifdef MALLOC
#  include "utils/malloc.hpp"
#endif
#include "utils/timer.hpp"
#include "utils/minmax.hpp"


template <typename float_ = double>
struct pair2
{
  float_ l;
  float_ u;
};


template <typename float_ = double>
struct pair2a
{
  float_ x;
  float_ a;
};



template <typename float_ = double>
void
_dp_line_c3(const int n,
            const float_ *y,
            const float_ lam,
            float_ *beta,
            pair2a<float_> *deriv,
            pair2<float_>  *bounds)
{
    const float_ mu = float_(1.0);
    int l, r, i;
    float_ a_, b_;

    if (n <= 1) {
        beta[0] = y[0];
        return;
    }

    {   Timer _ ("forward");
        l = n-1;       // leftest index in queue
        r = n-0;       // rightest index in queue
        deriv[l].x = bounds[0].l = -lam/mu + y[0];
        deriv[r].x = bounds[0].u = +lam/mu + y[0];
        deriv[l].a = +mu;
        deriv[r].a = -mu;

        for (i = 1; i < n-1; i++) {
            // clip from lower
            a_ = +mu;
            b_ = -mu*y[i] - lam;
            while (l <= r && a_ * deriv[l].x + b_ <= -lam) {
                b_ += -deriv[l].a * deriv[l].x;
                a_ += deriv[l].a;
                l += 1;
            }
            l -= 1;
            bounds[i].l = deriv[l].x = (-lam - b_) / a_;
            deriv[l].a = a_;
            // b[l] = b_ + lam;

            // clip from upper: a_ and b_ are negated (direction)
            a_ = -mu;               // negated!
            b_ = +mu * y[i] - lam;  // negated!
            while (l <= r && -(a_ * deriv[r].x + b_) >= lam) {
                b_ += -deriv[r].a * deriv[r].x;
                a_ += deriv[r].a;
                r -= 1;
            }
            r += 1;
            bounds[i].u = deriv[r].x = - (lam + b_) / a_;        // a_ and b_ negated!
            deriv[r].a = a_;
        }
    }
    {   Timer _ ("backward");
        // clip from below to 0
        a_ = mu;
        b_ = -mu * y[n-1] - lam;
        while (l <= r && a_ * deriv[l].x + b_ <= 0) {
            b_ += -deriv[l].a * deriv[l].x;
            a_ += deriv[l].a;
            l += 1;
        }
        beta[n-1] = -b_ / a_;
        b_ = beta[n-1];
        // back-pointers
        for (i = n-2; i >= 0; i--) {
            b_ = min(b_, bounds[i].u);
            b_ = max(b_, bounds[i].l);
            beta[i] = b_;
        }
    }
}


template <typename float_>
void
dp_line_c3(const int n,
          const float_ *y,
          const float_ lam,
          float_ *beta)
{
    std::vector<pair2a<float_>> deriv;
    std::vector<pair2<float_>> bounds;
    {   Timer _ ("alloc");
        deriv.reserve(n);
        bounds.reserve(n);
    }
    _dp_line_c3(n, y, lam, beta, deriv.data(), bounds.data());
}


template
void
dp_line_c3(const int n,
          const double *y,
          const double lam,
          double *beta);
