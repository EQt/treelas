#include <doctest/doctest.h>
#include <vector>

#include "../line_dp.hpp"
#include "utils/timer.hpp"


TEST_CASE("line_dp: line00")
{
    using doctest::Approx;

    const std::vector<double> y {1, 2, 1};
    const auto lam = ConstantWeights<>(0.1);
    std::vector<double> x;
    x.resize(y.size());

    {
        TimerQuiet _;
        line_las(y.size(), x.data(), y.data(), lam);
    }
    CHECK(1.1 == Approx(x[0]));
    CHECK(1.8 == Approx(x[1]));
    REQUIRE(1.1 == Approx(x[2]));
}
