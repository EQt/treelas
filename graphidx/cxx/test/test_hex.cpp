#include <gtest/gtest.h>
#include "../bits/hex.hpp"


TEST(hex, hex_u32)
{
    ASSERT_EQ(hex(0xfau),       "0xfa");
    ASSERT_EQ(hex(0xaabbccu),   "0xaabbcc");
    ASSERT_EQ(hex(0x12345678u), "0x12345678");
}


TEST(hex, hex_i32)
{
    ASSERT_EQ(hex(0xfa),       "0xfa");
    ASSERT_EQ(hex(0xaabbcc),   "0xaabbcc");
    ASSERT_EQ(hex(0x12345678), "0x12345678");
}


TEST(hex, hex_u64)
{
    ASSERT_EQ(hex(uint64_t(0xfa)),       "0xfa");
    ASSERT_EQ(hex(uint64_t(0xaabbcc)),   "0xaabbcc");
    ASSERT_EQ(hex(uint64_t(0x12345678)), "0x12345678");
}


TEST(hex, hex_i64)
{
    ASSERT_EQ(hex(int64_t(0xfa)),       "0xfa");
    ASSERT_EQ(hex(int64_t(0xaabbcc)),   "0xaabbcc");
    ASSERT_EQ(hex(int64_t(0x12345678)), "0x12345678");
}


TEST(hex, hex_f64)
{
    ASSERT_EQ(hex(1.0), "f64(0x3ff0000000000000)");
    ASSERT_EQ(hex(0.1), "f64(0x3fb999999999999a)");
    ASSERT_EQ(hex(2.0), "f64(0x4000000000000000)");
}


TEST(hex, hex_f32)
{
    ASSERT_EQ(hex(1.0f), "f32(0x3f800000)");
    ASSERT_EQ(hex(0.1f), "f32(0x3dcccccd)");
    ASSERT_EQ(hex(2.0f), "f32(0x40000000)");
}
