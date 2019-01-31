#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include "../bits/neg.hpp"


TEST(neg, negneg)
{
    ASSERT_EQ(neg(neg(int64_t(123))), int64_t(123));
    ASSERT_EQ(neg(neg(int8_t(123))), int8_t(123));
    ASSERT_EQ(neg(neg(int32_t(123))), int32_t(123));
}


TEST(neg, not_same)
{
    ASSERT_NE(neg(int64_t(123)), int64_t(123));
    ASSERT_NE(neg(int8_t(123)), int8_t(123));
    ASSERT_NE(neg(int32_t(123)), int32_t(123));
}


TEST(neg, digits)
{
    ASSERT_EQ(std::numeric_limits<uint8_t>::digits, 8);
    ASSERT_EQ(std::numeric_limits<int8_t>::digits,  7);
    ASSERT_EQ(std::numeric_limits<int16_t>::digits, 15);
    ASSERT_EQ(std::numeric_limits<int32_t>::digits, 31);
    ASSERT_EQ(std::numeric_limits<int64_t>::digits, 63);
}
