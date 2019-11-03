#include <doctest/doctest.h>
#include <vector>
#include <graphidx/bits/weights.hpp>

#include "../clip.hpp"
#include "../line_dp.hpp"


TEST_CASE("line_las")
{
    using doctest::Approx;

    SUBCASE("line_las3")
    {
        std::vector<double> y {1.0, 0.0, 0.5};
        const size_t n = y.size();
        REQUIRE(3 == n);

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
                REQUIRE(2 == i);
                REQUIRE(0.5 == y[i]);
                lb[i-1] = clip<+1, false>(event, pq, mu, -mu*y[i]-off + lam);
                REQUIRE(0.0 == lb[i-1]);
                REQUIRE(Range({int(n-1), int(n-1)}) == pq);
                REQUIRE(Event({0.0, 1.0}) == event[pq.start]);
                ub[i-1] = clip<-1, false>(event, pq, -mu, +mu*y[i] -off + lam);
                REQUIRE(1.0 == ub[i-1]);
                REQUIRE(Range({int(n-1), int(n)}) == pq);
                REQUIRE(Event({1.0, -1.0}) == event[pq.stop]);
                off = lam;
            }
            { // i = 1
                const size_t i = n-2;
                REQUIRE(1 == i);
                REQUIRE(0.0 == y[i]);
                lb[i-1] = clip<+1, false>(event, pq, mu, -mu*y[i]-off + lam);
                REQUIRE(Range({int(n-2), int(n)}) == pq);
                REQUIRE(0.0 == lb[i-1]);
                ub[i-1] = clip<-1, false>(event, pq, -mu, +mu*y[i]-off + lam);
            }
        }
    }


    SUBCASE("test_line.py: line1 with checks")
    {
        const std::vector<double> y {1.0, 0.0, 0.5};
        ConstantWeights<double> lam (0.5);
        auto x = line_las(y, lam);
        for (size_t i = 0; i < y.size(); i++) {
            INFO(i);
            CHECK(Approx(0.5) == x[i]);
        }
        CHECK(false);
    }


    SUBCASE("test_line.py: line1 without checks")
    {
        const std::vector<double> y {1.0, 0.0, 0.5};
        ConstantWeights<double> lam (0.5);
        std::vector<double> x;
        x.resize(y.size());
        line_las<double, ConstantWeights<double>, UnitWeights<double>, false>(
            y.size(), x.data(), y.data(), lam, UnitWeights<double>()
        );
        for (size_t i = 0; i < y.size(); i++) {
            INFO(i);
            CHECK(Approx(0.5) == x[i]);
        }
    }
}
