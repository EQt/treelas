#include <gtest/gtest.h>
#include "../cxx/line.hpp"
#include "../cxx/clip2.cpp"


TEST(Line, line_las3)
{
    std::vector<double> y {1.0, 0.0, 0.5};
    const size_t n = y.size();
    ASSERT_EQ(n, 3);

    const double lam = 0.5;
    std::vector<Event2> event_ (2*n);
    std::vector<double> lb_ (n-1), ub_ (n-1);
    {
            const double mu = 1.0;
            Event2 *event = event_.data();
            double *lb = lb_.data(), *ub = ub_.data();
            Queue pq {int(n), int(n-1)};
            { // i = 2
                const size_t i = n-1;
                ASSERT_EQ(i, 2);
                lb[i-1] = clip_front(event, pq, mu, -mu*y[i] -lam, -lam);
                ub[i-1] = clip_back (event, pq, mu, -mu*y[i] +lam, +lam);
            }
    }
}
