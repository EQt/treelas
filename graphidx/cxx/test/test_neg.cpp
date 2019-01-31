#include <gtest/gtest.h>
#include <cstdint>
#include "../bits/neg.hpp"


TEST(neg, negneg)
{
    ASSERT_EQ(neg(neg(int64_t(123))), int64_t(123));
    ASSERT_EQ(neg(neg(int8_t(123))), int8_t(123));
    ASSERT_EQ(neg(neg(int32_t(123))), int32_t(123));
}
