#include <gtest/gtest.h>
#include <vector>
#include "../tree/van_emde_boas.hpp"


TEST(veb, tree_h2)
{
    const int height = 2;
    const int n = (1 << height) - 1;
    std::vector<int> in ({1, 0, 2}), out (n, -1);

    build_hp_tree(in.data(), out.data(), height, 1);
    ASSERT_EQ(out, std::vector<int>({0, 1, 2}));
}


TEST(veb, tree_h3)
{
    const int height = 3;
    const int n = (1 << height) - 1;
    std::vector<int>
        out (n, -1),
        in ({3, 1, 4, 0, 5, 2, 6});
    ASSERT_EQ(in.size(), n);
    build_hp_tree(in.data(), out.data(), height, 1);
    ASSERT_EQ(out, std::vector<int>({0, 1, 3, 4, 2, 5, 6}));
}


TEST(veb, tree_h4)
{
    const int height = 4;
    const int n = (1 << height) - 1;
    std::vector<int>
        out (n, -1),
        in ({7, 3, 8, 1, 9, 4, 10, 0, 11, 5, 12, 2, 13, 6, 14});

    ASSERT_EQ(in.size(), n);
    build_hp_tree(in.data(), out.data(), height, 1);
    ASSERT_EQ(out, std::vector<int>({0,
                                     1,2,
                                     3, 
                                     7, 8,
                                     4,
                                     9, 10,
                                     5,
                                     11, 12,
                                     6,
                                     13, 14}));
}
