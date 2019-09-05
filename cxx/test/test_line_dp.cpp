#include <gtest/gtest.h>
#include <vector>

#include "../line_dp.hpp"


TEST(line_dp, line00)
{
    const std::vector<double> y {1, 2, 1};
    const auto lam = ConstantWeights<>(0.1);
    std::vector<double> x;
    x.resize(y.size());

    line_las(y.size(), x.data(), y.data(), lam);
    EXPECT_DOUBLE_EQ(x[0], 1.1);
    EXPECT_DOUBLE_EQ(x[1], 1.8);
    ASSERT_DOUBLE_EQ(x[2], 1.1);
}
