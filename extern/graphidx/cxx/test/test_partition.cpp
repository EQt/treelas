#include <gtest/gtest.h>
#include <set>
#include "../idx/partition.hpp"


TEST(partition, num5) {
    //                     0  1  2  3  4
    std::vector<int> num5 {1, 2, 0, 0, 2};
    PartitionIndex<> pidx {num5};

    EXPECT_EQ(pidx[0], std::set<int>({2, 3}));
    EXPECT_EQ(pidx[1], std::set<int>({0}));
    EXPECT_EQ(pidx[2], std::set<int>({1, 4}));
}
