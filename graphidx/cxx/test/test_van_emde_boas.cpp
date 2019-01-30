#include <gtest/gtest.h>
#include <vector>
#include "../tree/van_emde_boas.hpp"


TEST(veb, tree3)
{
    const int n = 3;
    std::vector<int> in (n), out (n, -1);
    for (int i = 0; i < n; i++)
        in[i] = i;

    build_hp_tree(in.data(), out.data(), 2, 1);
    ASSERT_EQ(in,  std::vector<int>({0, 1, 2}));
    ASSERT_EQ(out, std::vector<int>({0, 1, 2}));
}
