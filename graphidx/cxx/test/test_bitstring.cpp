#include <gtest/gtest.h>
#include "../bits/bitstring.hpp"


TEST(bitstring, std_bitset1)
{
    std::bitset<8> bits (0x81);
    ASSERT_EQ(bits.to_string(), "10000001") << bits.to_string();
}
