/**
   Aim at being more cache efficient than others
 */
#include "line_c.hpp"

#include <vector>
#include <stdexcept>

#include <graphidx/utils/timer.hpp>
#include <graphidx/bits/minmax.hpp>


template <int step, int size, typename T = double>
struct Slice
{
    T *const elem;

    inline T& operator[](size_t i) const { return elem[size*i + step]; }
};


template <typename float_ = double>
struct PackedBuf
{
    float_ *const beta;
    const Slice<0, 2, float_> x;
    const Slice<1, 2, float_> a;
    const Slice<0, 2, float_> lb;
    const Slice<1, 2, float_> ub;
    inline float_ b(size_t i) const { return -a[i] * x[i]; }
};


template <typename float_ = double>
struct SeqBuf
{
    float_ *const beta;
    float_ *const x;
    float_ *const a;
    float_ *const lb;
    float_ *const ub;

    inline float_ b(size_t i) const { return -a[i] * x[i]; }
};


template <typename Mem, typename float_ = double>
void
dp_line_c (
    const int n,
    const float_ *y,
    const float_ lam,
    Mem mem)
{
    constexpr float_ mu = float_(1.0);
    int l, r, i;
    float_ a_, b_;

    if (n <= 1) {
        mem.beta[0] = y[0];
        return;
    }

    {   Timer _ ("forward");
        l = n-1;       // leftest indemem.x in queue
        r = n-0;       // rightest indemem.x in queue
        mem.x[l] = mem.lb[0] = -lam/mu + y[0];
        mem.x[r] = mem.ub[0] = +lam/mu + y[0];
        mem.a[l] = +mu;
        mem.a[r] = -mu;
        // mem.b[l] = -mu*y[0] + lam;
        // mem.b[r] = +mu*y[0] + lam;

        for (i = 1; i < n-1; i++) {
            // clip from lower
            a_ = +mu;
            b_ = -mu*y[i] - lam;
            while (l <= r && a_ * mem.x[l] + b_ <= -lam) {
                a_ += mem.a[l];
                b_ += mem.b(l);
                l += 1;
            }
            l -= 1;
            mem.lb[i] = mem.x[l] = (-lam - b_) / a_;
            mem.a[l] = a_;
            // mem.b[l] = b_ + lam;

            // clip from upper: a_ and b_ are negated (direction)
            a_ = -mu;               // negated!
            b_ = +mu * y[i] - lam;  // negated!
            while (l <= r && -(a_ * mem.x[r] + b_) >= lam) {
                a_ += mem.a[r];
                b_ += mem.b(r);
                r -= 1;
            }
            r += 1;
            mem.ub[i] = mem.x[r] = - (lam + b_) / a_;        // a_ and b_ negated!
            mem.a[r] = a_;
            // mem.b[r] = b_ + lam;
        }
    }
    {   Timer _ ("backward");
        // clip from below to 0
        a_ = mu;
        b_ = -mu * y[n-1] - lam;
        while (l <= r && a_ * mem.x[l] + b_ <= 0) {
            a_ += mem.a[l];
            b_ += mem.b(l);
            l += 1;
        }
        mem.beta[n-1] = -b_ / a_;
        b_ = mem.beta[n-1];
        // back-pointers
        for (i = n-2; i >= 0; i--) {
            b_ = min(b_, mem.ub[i]);
            b_ = max(b_, mem.lb[i]);
            mem.beta[i] = b_;
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
#if false
    std::vector<float_> buf;
    {
        Timer _ ("alloc");
        buf.reserve(2*n + 2*n + n + n);
    }
    size_t p = 0;
    auto x = buf.data() + p; p += 2*n;
    auto a = buf.data() + p; p += 2*n;
    auto lb = buf.data() + p; p += n;
    auto ub = buf.data() + p; p += n;
    if (p > buf.capacity())
        throw std::runtime_error(
            std::string("dp_line_c(): ERROR during allocation: ") +
            "p = " + std::to_string(p) + " != " +
            std::to_string(buf.capacity()) + " = buf.capacity()");
    SeqBuf<float_> mem {
        beta,
        x,
        a,
        lb,
        ub
    };
    dp_line_c(n, y, lam, mem);
#else
    std::vector<float_> buf;
    {
        Timer _ ("alloc");
        buf.reserve(2*n + 2*n + n + n);
    }
    PackedBuf<float_> mem {
        beta,
        {buf.data()},
        {buf.data()},
        {buf.data() + 4*n},
        {buf.data() + 4*n},
    };
    dp_line_c(n, y, lam, mem);
#endif

#if false
    std::vector<float_> x, a, b, lb, ub;
    {   Timer _ ("alloc");
        x.reserve(2*n);
        a.reserve(2*n);
        b.reserve(2*n);
        lb.reserve(n);
        ub.reserve(n);
    }
    dp_line_c(n, y, lam, beta, x.data(),
               a.data(), b.data(), lb.data(), ub.data());
#endif
}


template
void
dp_line_c(const int n,
          const double *y,
          const double lam,
          double *beta);
