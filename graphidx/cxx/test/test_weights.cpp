#include <gtest/gtest.h>
#include "../bits/weights.hpp"


TEST(weights, constant0)
{
    auto w = create_weight(13.5);
    ASSERT_EQ(w[5], 13.5);
    ASSERT_EQ(w[0], 13.5);
}
