/**
   Aim at being more cache efficient than others
*/
#include "cline2.hpp"

#include <vector>
#include <stdexcept>

#ifdef MALLOC
#  include "utils/malloc.hpp"
#endif
#include "utils/timer.hpp"
#include "utils/minmax.hpp"


template <typename float_ = double>
void
_dp_line_c2(const int n,
            const float_ *y,
            const float_ lam,
            float_ *beta,
            float_ *x,
            float_ *a,
            float_ *ub)
{
    float_ *lb = beta;

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
        x[l] = lb[0] = -lam/mu + y[0];
        x[r] = ub[0] = +lam/mu + y[0];
        a[l] = +mu;
        a[r] = -mu;
        // b[l] = -mu*y[0] + lam;
        // b[r] = +mu*y[0] + lam;

        for (i = 1; i < n-1; i++) {
            // clip from lower
            a_ = +mu;
            b_ = -mu*y[i] - lam;
            while (l <= r && a_ * x[l] + b_ <= -lam) {
                b_ += -a[l] * x[l];
                a_ += a[l];
                l += 1;
            }
            l -= 1;
            lb[i] = x[l] = (-lam - b_) / a_;
            a[l] = a_;
            // b[l] = b_ + lam;

            // clip from upper: a_ and b_ are negated (direction)
            a_ = -mu;               // negated!
            b_ = +mu * y[i] - lam;  // negated!
            while (l <= r && -(a_ * x[r] + b_) >= lam) {
                b_ += -a[r] * x[r];
                a_ += a[r];
                r -= 1;
            }
            r += 1;
            ub[i] = x[r] = - (lam + b_) / a_;        // a_ and b_ negated!
            a[r] = a_;
        }
    }
    {   Timer _ ("backward");
        // clip from below to 0
        a_ = mu;
        b_ = -mu * y[n-1] - lam;
        while (l <= r && a_ * x[l] + b_ <= 0) {
            b_ += -a[l] * x[l];
            a_ += a[l];
            l += 1;
        }
        beta[n-1] = -b_ / a_;
        b_ = beta[n-1];
        // back-pointers
        for (i = n-2; i >= 0; i--) {
            b_ = min(b_, ub[i]);
            b_ = max(b_, lb[i]);
            beta[i] = b_;
        }
    }
}


template <typename float_>
void
dp_line_c2(const int n,
          const float_ *y,
          const float_ lam,
          float_ *beta)
{
    Timer t ("alloc");
#ifdef BLOCK_ALLOC
#  ifdef MALLOC
    Malloc<float_> buf (2*n + 2*n + n);
#  else
    std::vector<float_> buf (2*n + 2*n + n);
#endif
    size_t p = 0;
    float_ *x = buf.data() + p; p += 2*n;
    float_ *a = buf.data() + p; p += 2*n;
    float_ *ub = buf.data() + p; p += n;
    t.stop();
    if (p != buf.size())
        throw std::runtime_error("Should not happen");
    _dp_line_c2(n, y, lam, beta, x, a, ub);
#else
    std::vector<float_>
        x (2*n),
        a (2*n),
        ub (n);
    t.stop();
    _dp_line_c2(n, y, lam, beta,
                x.data(), a.data(), ub.data());
#endif
}


template
void
dp_line_c2(const int n,
          const double *y,
          const double lam,
          double *beta);
