#include <gtest/gtest.h>
#include "../bits/bitstring.hpp"


/** Shift 0b1 to the left. */
inline uint64_t
shift1u64(int n)
{
    return uint64_t(1) << n;
}


/** Like shift1u64 but with signed ints (to see whether it makes a difference). */
inline int
shift1int(int n)
{
    return int(1) << n;
}


TEST(shift, shift_u64)
{
    // https://stackoverflow.com/a/7637
    // https://en.wikibooks.org/wiki/X86_Assembly/Shift_and_Rotate
    //   SAR (shift arithmetic right, or signed shift) vs
    //   SHR (shift [logical], or unsigned shift)
    //
    {
        uint64_t n = 64;
        EXPECT_EQ((0x8000000000000000ULL >> (n-1)) >> 1, 0);
        EXPECT_EQ((0x8000000000000000ULL >> 63) >> 1, 0);
        // undefined behavior: depends on the compiler
        // EXPECT_EQ((0x8000000000000000ULL >> (64)) >> 1, 0ULL);
    }

    EXPECT_EQ(shift1u64(63 - (int)64), uint64_t(0));
    EXPECT_EQ(shift1u64(63), 0x8000000000000000);
    // const uint64_t nums[] = {0};
    // EXPECT_EQ(shift1u64(63) >> leading_zeros(nums[0]), uint64_t(0))
    //     << bitstring(shift1u64(63) >> leading_zeros(nums[0]));
}


TEST(shift, neg_int)
{
    EXPECT_EQ(shift1int(-1), 0) << bitstring(shift1int(-1));
    int n = 64;
    EXPECT_EQ(shift1int(63 - n), 0) << bitstring(shift1int(63 - n));
}


TEST(shift, neg_u64)
{
    EXPECT_EQ(shift1u64(-1), uint64_t(0))
        << bitstring(shift1u64(-1));

    {
        const int n = 64;
        EXPECT_EQ(shift1u64(63 - n), uint64_t(0))
            << bitstring(shift1u64(63 - n));
    }
}
