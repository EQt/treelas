#include <gtest/gtest.h>
#include "../bits/bitstring.hpp"


TEST(bitstring, std_bitset1)
{
    std::bitset<8> bits {0x81};
    ASSERT_EQ(bits.to_string(), "10000001");
}


TEST(bitstring, std_bitset2)
{
    std::bitset<8> bits {std::string("10101111")};
    ASSERT_EQ(bits.to_string(), "10101111");
}


TEST(bitstring, bitset_u8)
{
    ASSERT_EQ(bitstring(uint8_t(0x81)), "10000001");
}


TEST(bitstring, bitset_i8)
{
    ASSERT_EQ(bitstring(int8_t(-1)), "11111111");
}


TEST(bitstring, bitset_i32_neg)
{
    ASSERT_EQ(bitstring(int32_t(-1)), "11111111111111111111111111111111");
}


TEST(bitstring, bitset_i32_long)
{
    ASSERT_EQ(bitstring(int32_t(0x11223344)), "00010001001000100011001101000100");
}
