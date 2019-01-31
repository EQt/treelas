#include <gtest/gtest.h>
#include <vector>
#include "../tree/inorder.hpp"
#include "../tree/van_emde_boas.hpp"


TEST(veb, tree_h2)
{
    const int height = 2;
    const int n = (1 << height) - 1;
    const std::vector<int> in = binary_inorder(height);
    std::vector<int> out (n, -1);

    binveb_from_inord(out.data(), in.data(), height);
    ASSERT_EQ(out, std::vector<int>({0, 1, 2}));
}


TEST(veb, tree_h3)
{
    const int height = 3;
    const int n = (1 << height) - 1;
    const std::vector<int> in = binary_inorder(height);
    std::vector<int> out (n, -1);
    ASSERT_EQ(in.size(), n);
    binveb_from_inord(out.data(), in.data(), height, 1);
    ASSERT_EQ(out, std::vector<int>({0, 1, 3, 4, 2, 5, 6}));
}


TEST(veb, tree_h4)
{
    const int height = 4;
    const int n = (1 << height) - 1;
    const std::vector<int> in = binary_inorder(height);
    std::vector<int> out (n, -1);

    ASSERT_EQ(in.size(), n);
    binveb_from_inord(out.data(), in.data(), height);
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
