#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include "../bits/neg.hpp"


TEST(neg, negneg)
{
    ASSERT_EQ(neg(neg(int64_t(123))), int64_t(123));
    ASSERT_EQ(neg(neg(int8_t(123))), int8_t(123));
    ASSERT_EQ(neg(neg(int32_t(123))), int32_t(123));
    ASSERT_EQ(neg(neg(int32_t(0))), int32_t(0));
}


TEST(neg, not_same)
{
    ASSERT_NE(neg(int64_t(123)), int64_t(123));
    ASSERT_NE(neg(int8_t(123)), int8_t(123));
    ASSERT_NE(neg(int32_t(123)), int32_t(123));
    ASSERT_NE(neg(int64_t(0)), int64_t(0));

}


TEST(neg, digits)
{
    ASSERT_EQ(std::numeric_limits<uint8_t>::digits, 8);
    ASSERT_EQ(std::numeric_limits<int8_t>::digits,  7);
    ASSERT_EQ(std::numeric_limits<int16_t>::digits, 15);
    ASSERT_EQ(std::numeric_limits<int32_t>::digits, 31);
    ASSERT_EQ(std::numeric_limits<int64_t>::digits, 63);
}


TEST(neg, is_pos)
{
    ASSERT_TRUE(is_pos(int32_t(34)));
    ASSERT_TRUE(is_pos(int32_t(0)));
    ASSERT_FALSE(is_pos(neg(int32_t(0))));
    ASSERT_FALSE(is_pos(neg(int64_t(34))));
}
