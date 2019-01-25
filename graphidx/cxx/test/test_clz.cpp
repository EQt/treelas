#include <gtest/gtest.h>
#include "../bits/clz.hpp"
#include "../bits/hex.hpp"


TEST(clz, clz_u32)
{
    const auto n = 8;
    uint32_t nums[n] = {
        0xc88dbe40,
        0x45f513c1,
        0x0245e20e,
        0x0001f134,
        0x0000000d,
        0x00000000,
        0x00000001,
        0xffffffff
    };
    size_t expect[n] = {0, 1, 6, 15, 28, 32, 31, 0};
    for (int i = 0; i < n; i++)
        ASSERT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << nums[i]
            << " expect[" << i << "]" << expect[i];
}


TEST(clz, clz_i32)
{
    const int n = 8;
    int32_t nums[n] = {-930234816, 1173689281, 38134286, 127284, 13, 0, 1, -1};
    size_t expect[n] = {0, 1, 6, 15, 28, 32, 31, 0};
    for (int i = 0; i < n; i++)
        ASSERT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << nums[i]
            << " expect[" << i << "]" << expect[i];
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
        ASSERT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << nums[i]
            << " expect[" << i << "]" << expect[i];
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
        ASSERT_EQ(leading_zeros(nums[i]), expect[i])
            << "nums[" << i << "] = " << nums[i];
}


TEST(clz, hyperfloor_shiftneg_int)
{
    int n = -1;
    ASSERT_EQ(1 << n, 0);
    n = 64;
    ASSERT_EQ(1 << (63 - n), 0);
}


TEST(clz, hyperfloor_shiftneg_u64)
{
    int n = -1;
    ASSERT_EQ(uint64_t(1) << n, uint64_t(0));
    n = 64;
    ASSERT_EQ(uint64_t(1) << (63 - n), uint64_t(0));
}


TEST(clz, hyperfloor_u64)
{
    const auto n = 8;
    uint64_t nums[n] = {
        0xc6d9987dc7329e40, // 0
        0x71af2ce190c639d6,
        0x03d2bfb96a9c418e, // 2
        0x000166c7881035f9,
        0x0000000eb3127caf, // 4
        0x00000000000576b5,
        0x0000000000000001, // 6
        0xffffffffffffffff
    };

    uint64_t expect[n] = {
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
        ASSERT_EQ(hyperfloor(nums[i]), expect[i])
            << "nums[" << i << "] = " << hex(nums[i])
            << " expect[" << i << "] = " << hex(expect[i])
            << " hyperfloor = " << hex(hyperfloor(nums[i]))
            << " clz = " << leading_zeros(nums[i]);
}


TEST(clz, hyperfloor_u64_0)
{
    ASSERT_EQ(hyperfloor(uint64_t(0)), uint64_t(0));
}
