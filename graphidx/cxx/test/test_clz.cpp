#include <gtest/gtest.h>
#include "../bits/clz.hpp"
#include "../bits/hex.hpp"
#include "../bits/bitstring.hpp"


TEST(clz, clz_u32)
{
    const auto n = 8;
    uint32_t nums[n] = {
                    //     0        8       16       24
        0xc88dbe40, // 0 0b11001000'10001101'10111110'01000000
        0x45f513c1, //   0b01000101'11110101'00010011'11000001
        0x0245e20e, // 2 0b00000010'01000101'11100010'00001110
        0x0001f134, //   0b00000000'00000001'11110001'00110100
        0x0000000d, // 4 0b00000000'00000000'00000000'00001101
        0x00000000, //   0b00000000'00000000'00000000'00000000
        0x00000001, // 6 0b00000000'00000000'00000000'00000001
        0xffffffff  //   0b11111111'11111111'11111111'11111111
    };
    ASSERT_EQ(nums[0],  0b110010001'00011011'01111100'1000000)
        << "nums[0]" << nums[0];

    size_t expect[n] = {0, 1, 6, 15, 28, 32, 31, 0};
    for (int i = 0; i < n; i++)
        EXPECT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << bitstring(nums[i]);
}


TEST(clz, clz_i32)
{
    const int n = 8;
    int32_t nums[n] = {-930234816, 1173689281, 38134286, 127284, 13, 0, 1, -1};
    size_t expect[n] = {0, 1, 6, 15, 28, 32, 31, 0};
    for (int i = 0; i < n; i++)
        EXPECT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << bitstring(nums[i]);
}


TEST(clz, clz_u64)
{
    const auto n = 9;
    uint64_t nums[n] = {
        0xc6d9987dc7329e40, // 0
        0x71af2ce190c639d6,
        0x03d2bfb96a9c418e, // 2
        0x000166c7881035f9,
        0x0000000eb3127caf, // 4
        0x00000000000576b5,
        0x0000000000000001, // 6
        0xffffffffffffffff,
        0x0000000000000000  // 8
    };

    size_t expect[n] = {0,
                        1,
                        6,
                        15,
                        28,
                        45,
                        63,
                        0,
                        64};
    for (int i = 0; i < n; i++)
        EXPECT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << bitstring(nums[i]);
}


TEST(clz, clz_i64)
{
    const auto n = 8;
    int64_t nums[n] = {
        0x71af2ce190c639d6,
        0x03d2bfb96a9c418e,
        0x000166c7881035f9,
        0x0000000eb3127caf,
        0x00000000000576b5,
        0x0000000000000001,
        -1, 0
    };

    size_t expect[n] = {1, 6, 15, 28, 45, 63, 0, 64};
    for (int i = 0; i < n; i++)
        EXPECT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << bitstring(nums[i]);
}


TEST(clz, hyperfloor_shiftneg_int)
{
    EXPECT_EQ(shift1int(-1), 0) << bitstring(shift1int(-1));
    int n = 64;
    EXPECT_EQ(shift1int(63 - n), 0) << bitstring(shift1int(63 - n));
}


TEST(clz, hyperfloor_shiftneg_u64)
{
    EXPECT_EQ(shift1u64(-1), uint64_t(0))
        << hex(shift1u64(-1));

    {
        const int n = 64;
        EXPECT_EQ(shift1u64(63 - n), uint64_t(0))
            << hex(shift1u64(63 - n));
    }
    EXPECT_EQ(leading_zeros(uint64_t(0)), 64);
    EXPECT_EQ(shift1u64(63 - (int)leading_zeros(uint64_t(0))), uint64_t(0));
}


TEST(clz, hyperfloor_u64)
{
    const auto n = 8;
    const uint64_t nums[n] = {
        0xc6d9987dc7329e40, // 0
        0x71af2ce190c639d6,
        0x03d2bfb96a9c418e, // 2
        0x000166c7881035f9,
        0x0000000eb3127caf, // 4
        0x00000000000576b5,
        0x0000000000000001, // 6
        0xffffffffffffffff
    };

    const uint64_t expect[n] = {
        0x8000000000000000,
        0x4000000000000000,
        0x0200000000000000,
        0x0001000000000000,
        0x0000000800000000,
        0x0000000000040000,
        0x0000000000000001,
        0x8000000000000000
    };

    for (int i = 0; i < n; i++)
        EXPECT_EQ(hyperfloor(nums[i]), expect[i])
            << "nums[" << i << "] = " << bitstring(nums[i])
            << "\nexpect[" << i << "] = " << bitstring(expect[i])
            << "\nhyperfloor = " << bitstring(hyperfloor(nums[i]))
            << "\nclz = " << leading_zeros(nums[i]);
}


TEST(clz, shift_u64)
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

    const uint64_t nums[] = {0};

    EXPECT_EQ(shift1u64(63 - (int)64), uint64_t(0));
    EXPECT_EQ(shift1u64(63), 0x8000000000000000);
    EXPECT_EQ(shift1u64(63) >> leading_zeros(nums[0]), uint64_t(0))
        << hex(shift1u64(63) >> leading_zeros(nums[0]));
}


TEST(clz, hyperfloor_u64_0)
{
    EXPECT_EQ(hyperfloor(uint64_t(0)), uint64_t(0));
    const uint64_t nums[] = {1, 0};
    EXPECT_EQ(nums[0], uint64_t(1));
    EXPECT_EQ(nums[1], uint64_t(0));
    EXPECT_EQ(leading_zeros(nums[1]), 64);
    EXPECT_EQ(hyperfloor(nums[1]), uint64_t(0))
        << hex(hyperfloor(nums[1]))
        << " leading_zeros = " << leading_zeros(nums[1]);
}
