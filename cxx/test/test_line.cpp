#include <doctest/doctest.h>
#include <vector>
#include <graphidx/bits/weights.hpp>
#include <graphidx/utils/timer.hpp>

#define DEBUG_CLIP false
#include "../clip.hpp"
#include "../line_dp.hpp"


TEST_CASE("line_las")
{
    using doctest::Approx;
    TimerQuiet _;

    SUBCASE("test_line.py: line1 with checks")
    {
        const std::vector<double> y {1.0, 0.0, 0.5};
        ConstantWeights<double> lam (0.5);
        REQUIRE(y.size() == 3);
        auto x = line_las(y, lam);
        for (size_t i = 0; i < y.size(); i++) {
            INFO(i);
            CHECK(Approx(0.5) == x[i]);
        }
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
