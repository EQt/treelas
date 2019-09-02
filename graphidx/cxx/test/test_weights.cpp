#include <gtest/gtest.h>
#include "../bits/weights.hpp"


TEST(weights, constant13)
{
    auto w = create_weight(13.5);
    ASSERT_EQ(w[5], 13.5);
    ASSERT_EQ(w[0], 13.5);
}


TEST(weights, array123)
{
    const int a[] = {1, 2, 5};
    auto w = create_weight(a);
    ASSERT_EQ(w[2], 5);
    ASSERT_EQ(w[0], 1);
    ASSERT_EQ(w[1], 2);
}
