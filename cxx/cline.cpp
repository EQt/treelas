/**
   Aim at being more cache efficient than others
*/
#include <vector>
#include "utils/timer.hpp"


inline double 
min(double a, double b) {
    return a < b ? a : b;
}


inline double 
max(double a, double b) {
    return a > b ? a : b;
}


template <typename float_ = double>
void
_dp_line_c (const int n,
            const float_ *y,
            const float_ lam,
            float_ *beta, 
            float_ *x,
            float_ *a,
            float_ *b,
            float_ *kl,
            float_ *ku)
{
    const float_ mu = 1.0;
    int l, r, i;
    float_ a_, b_;

    if (n <= 1) {
        beta[0] = y[0];
        return;
    }

    {   Timer _ ("forward");

        l = n-1;       // leftest index in queue
        r = n-0;       // rightest index in queue
        x[l] = kl[0] = -lam/mu + y[0];
        x[r] = ku[0] = +lam/mu + y[0];
        a[l] = +mu;
        a[r] = -mu;
        b[l] = -mu*y[0] + lam;
        b[r] = +mu*y[0] + lam;

        for (i = 1; i < n-1; i++) {
            // clip from lower
            a_ = +mu;
            b_ = -mu*y[i] - lam;
            while (l <= r && a_ * x[l] + b_ <= -lam) {
                a_ += a[l];
                b_ += b[l];
                l += 1;
            }
            l -= 1;
            kl[i] = x[l] = (-lam - b_) / a_;
            a[l] = a_;
            b[l] = b_ + lam;

            // clip from upper: a_ and b_ are negated (direction)
            a_ = -mu;               // negated!
            b_ = +mu * y[i] - lam;  // negated!
            while (l <= r && -(a_ * x[r] + b_) >= lam) {
                a_ += a[r];
                b_ += b[r];
                r -= 1;
            }
            r += 1;
            ku[i] = x[r] = - (lam + b_) / a_;        // a_ and b_ negated!
            a[r] = a_;
            b[r] = b_ + lam;
        }
    }
    {   Timer _ ("backward");
            // clip from below to 0
        a_ = mu;
        b_ = -mu * y[n-1] - lam;
        while (l <= r && a_ * x[l] + b_ <= 0) {
            a_ += a[l];
            b_ += b[l];
            l += 1;
        }
        beta[n-1] = -b_ / a_;
        b_ = beta[n-1];
        // back-pointers
        for (i = n-2; i >= 0; i--) {
            b_ = min(b_, ku[i]);
            b_ = max(b_, kl[i]);
            beta[i] = b_;
        }
    }
}


template <typename float_>
void
dp_line_c(const int n,
          const float_ *y,
          const float_ lam,
          float_ *beta)
{
    Timer t ("alloc");
    std::vector<float_>
        x (2*n),
        a (2*n),
        b (2*n),
        lb (n-1),
        ub (n-1);
    t.stop();
    _dp_line_c(n, y, lam, beta,
               x.data(), a.data(), b.data(), lb.data(), ub.data());
}


template
void
dp_line_c(const int n,
          const double *y,
          const double lam,
          double *beta);
