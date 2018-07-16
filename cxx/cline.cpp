inline double 
min(double a, double b) {
    return a < b ? a : b;
}


inline double 
max(double a, double b) {
    return a > b ? a : b;
}


#ifdef __cplusplus
extern "C" {
#endif

void
tf_dp_weight2 (const int n, const double *y,
               const double lam, double *beta, 
               double *x, double *a, double *b, double *kl, double *ku)
{
    const double mu = 1.0;
    int l, r, i;
    double a_, b_;

    if (n <= 1) {
        beta[0] = y[0];
        return;
    }

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
        a_ = +w[i];
        b_ = -w[i]*y[i] - lam;
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
        a_ = -w[i];               // negated!
        b_ = +w[i] * y[i] - lam;  // negated!
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

    // clip from below to 0
    a_ = +w[n-1];
    b_ = -w[n-1] * y[n-1] - lam;
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

#ifdef __cplusplus
}
#endif


typedef struct {
  double l;
  double u;
} pair2;


typedef struct {
  double x;
  double a;
  double b;
} pair3;


#ifdef __cplusplus
extern "C" {
#endif

void
tf_dp_weight3 (const int n, const double *y, const double *w,
               const double lam, double *beta,
               pair3 *deriv, pair2 *bounds)
{
    int l, r, i;
    double a_, b_;

    if (n <= 1) {
        beta[0] = y[0];
        return;
    }

    l = n-1;       // leftest index in queue
    r = n-0;       // rightest index in queue
    deriv[l].x = bounds[0].l = -lam/mu + y[0];
    deriv[r].x = bounds[0].u = +lam/mu + y[0];
    deriv[l].a = +mu;
    deriv[r].a = -mu;
    deriv[l].b = -mu*y[0] + lam;
    deriv[r].b = +mu*y[0] + lam;

    for (i = 1; i < n-1; i++) {
        // clip from lower
        a_ = +w[i];
        b_ = -w[i]*y[i] - lam;
        while (l <= r && a_ * deriv[l].x + b_ <= -lam) {
            a_ += deriv[l].a;
            b_ += deriv[l].b;
            l += 1;
        }
        l -= 1;
        deriv[l].x = bounds[i].l = (-lam - b_) / a_;
        deriv[l].a = a_;
        deriv[l].b = b_ + lam;

        // clip from upper: a_ and b_ are negated (direction)
        a_ = -w[i];               // negated!
        b_ = +w[i] * y[i] - lam;  // negated!
        while (l <= r && -(a_ * deriv[r].x + b_) >= lam) {
            a_ += deriv[r].a;
            b_ += deriv[r].b;
            r -= 1;
        }
        r += 1;
        deriv[r].x = bounds[i].u = - (lam + b_) / a_;
        deriv[r].a = a_;
        deriv[r].b = b_ + lam;
    }

    // clip from below to 0
    a_ = +w[n-1];
    b_ = -w[n-1] * y[n-1] - lam;
    while (l <= r && a_ * deriv[l].x + b_ <= 0) {
        a_ += deriv[l].a;
        b_ += deriv[l].b;
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

void
tf_dp_weightl3 (const int n, const double *y,
                const double *w, const double* lam,
                double *beta,
                pair3 *deriv, pair2 *bounds)
{
    int l, r, i;
    double a_, b_;

    if (n <= 1) {
        beta[0] = y[0];
        return;
    }

    l = n-1;       // leftest index in queue
    r = n-0;       // rightest index in queue
    deriv[l].x = bounds[0].l = -lam[0]/mu + y[0];
    deriv[r].x = bounds[0].u = +lam[0]/mu + y[0];
    deriv[l].a = +mu;
    deriv[r].a = -mu;
    deriv[l].b = -mu*y[0] + lam[0];
    deriv[r].b = +mu*y[0] + lam[0];

    for (i = 1; i < n-1; i++) {
        // clip from lower
        a_ = +mu;
        b_ = -mu*y[i] - lam[i-1];
        while (l <= r && a_ * deriv[l].x + b_ <= -lam[i]) {
            a_ += deriv[l].a;
            b_ += deriv[l].b;
            l += 1;
        }
        l -= 1;
        deriv[l].x = bounds[i].l = (-lam[i] - b_) / a_;
        deriv[l].a = a_;
        deriv[l].b = b_ + lam[i];

        // clip from upper: a_ and b_ are negated (direction)
        a_ = -mu;                    // negated!
        b_ = +mu * y[i] - lam[i-1];  // negated!
        while (l <= r && -(a_ * deriv[r].x + b_) >= lam[i]) {
            a_ += deriv[r].a;
            b_ += deriv[r].b;
            r -= 1;
        }
        r += 1;
        deriv[r].x = bounds[i].u = - (lam[i] + b_) / a_;
        deriv[r].a = a_;
        deriv[r].b = b_ + lam[i];
    }

    // clip from below to 0
    a_ = +mu
    b_ = -mu * y[n-1] - lam[n-2];
    while (l <= r && a_ * deriv[l].x + b_ <= 0) {
        a_ += deriv[l].a;
        b_ += deriv[l].b;
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

#ifdef __cplusplus
}
#endif
