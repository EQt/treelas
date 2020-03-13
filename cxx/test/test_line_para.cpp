#include <doctest/doctest.h>
#include <sstream>
#include <vector>

#include "../line/line_para.hpp"
#include <graphidx/utils/timer.hpp>


TEST_CASE("line_para: line4 (not parallel)")
{
    const std::vector<double> y {-0.28,  0.58,  2.15, -1.28};
    const double lam = 0.01;
    std::vector<double> x (y.size(), -42.7);
    {
        TimerQuiet _;
        line_para(y.size(), y.data(), lam, x.data(), false);
    }
    const std::vector<double> x_opt {-0.27,  0.58,  2.13, -1.27};
    for (size_t i = 0; i < y.size(); i++) {
        INFO(i);
        CHECK(x_opt[i] == doctest::Approx(x[i]));
    }
}


TEST_CASE("line_para: line4 (parallel)")
{
    const std::vector<double> y {-0.28,  0.58,  2.15, -1.28};
    const double lam = 0.01;
    std::vector<double> x (y.size(), -42.7);
    {
        TimerQuiet _;
        line_para(y.size(), y.data(), lam, x.data(), true);
    }
    const std::vector<double> x_opt {-0.27,  0.58,  2.13, -1.27};
    for (size_t i = 0; i < y.size(); i++) {
        INFO(i);
        CHECK(x_opt[i] == doctest::Approx(x[i]));
    }
}
