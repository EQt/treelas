#include <gtest/gtest.h>
#include <vector>

#include "../line/line_dp.cpp"
#include "bits/weights.hpp"


TEST(line_dp, line00)
{
    const std::vector<double> y {1, 2, 1};
    const auto lam = ConstantWeights<>(0.1);
    std::vector<double> x;
    x.resize(y.size());

    line_las(y.size(), x.data(), y.data(), lam);
}
