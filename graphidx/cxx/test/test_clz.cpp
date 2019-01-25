#include <gtest/gtest.h>
#include "../bits/clz.hpp"


TEST(clz, clz_u32)
{
}


TEST(clz, clz_i32)
{
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
