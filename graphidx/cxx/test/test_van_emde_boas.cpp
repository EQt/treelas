#include <gtest/gtest.h>
#include <vector>
#include "../tree/van_emde_boas.hpp"


TEST(veb, tree_h2)
{
    const int height = 2;
    const int n = (1 << height) - 1;
    std::vector<int> in (n), out (n, -1);
    for (int i = 0; i < n; i++)
        in[i] = i;

    build_hp_tree(in.data(), out.data(), height, 1);
    ASSERT_EQ(in,  std::vector<int>({0, 1, 2}));
    ASSERT_EQ(out, std::vector<int>({0, 1, 2}));
}


TEST(veb, tree_h4)
{
    const int height = 4;
    const int n = (1 << height) - 1;
    std::vector<int> in (n), out (n, -1);
    for (int i = 0; i < n; i++)
        in[i] = i;

    build_hp_tree(in.data(), out.data(), height, 1);
    ASSERT_EQ(out, std::vector<int>({0,
                                     1,          2,
                                     3,    6,    9,      12,
                                     4, 5, 7, 8, 10, 11, 13, 14}));
}
