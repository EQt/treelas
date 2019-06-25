#include <gtest/gtest.h>
#include "../line/line.hpp"
#include "../clip.hpp"


TEST(Line, line_las3)
{
    std::vector<double> y {1.0, 0.0, 0.5};
    const size_t n = y.size();
    ASSERT_EQ(n, 3);

    const double lam = 0.5;
    std::vector<Event> event_ (2*n);
    std::vector<double> lb_ (n-1), ub_ (n-1);
    {
            const double mu = 1.0;
            Event *event = event_.data();
            double *lb = lb_.data(), *ub = ub_.data();
            Range pq {int(n), int(n-1)};
            double off = 0.0;
            { // i = 2
                const size_t i = n-1;
                ASSERT_EQ(i, 2);
                ASSERT_EQ(y[i], 0.5);
                lb[i-1] = clip_front(event, pq, mu, -mu*y[i]-off + lam);
                ASSERT_EQ(lb[i-1], 0.0);
                ASSERT_EQ(pq, Range({int(n-1), int(n-1)}));
                ASSERT_EQ(event[pq.start], Event({0.0, 1.0}));
                ub[i-1] = clip_back (event, pq, mu, -mu*y[i]+off, +lam);
                ASSERT_EQ(ub[i-1], 1.0);
                ASSERT_EQ(pq, Range({int(n-1), int(n)}));
                ASSERT_EQ(event[pq.stop], Event({1.0, -1.0}));
                off = lam;
            }
            { // i = 1
                const size_t i = n-2;
                ASSERT_EQ(i, 1);
                ASSERT_EQ(y[i], 0.0);
                lb[i-1] = clip_front(event, pq, mu, -mu*y[i]-off + lam);
                ASSERT_EQ(pq, Range({int(n-2), int(n)}));
                ASSERT_EQ(lb[i-1], 0.0);
                ub[i-1] = clip_back (event, pq, mu, -mu*y[i]+off, +lam);
            }
    }
}
