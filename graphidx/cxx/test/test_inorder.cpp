#include <gtest/gtest.h>
#include <vector>
#include "../tree/inorder.hpp"


TEST(inorder, t0)
{
    ASSERT_EQ(binary_inorder(0),
              std::vector<int>({}));
}


TEST(inorder, t1)
{
    ASSERT_EQ(binary_inorder(1),
              std::vector<int>({0}));
}


TEST(inorder, t2)
{
    ASSERT_EQ(binary_inorder(2),
              std::vector<int>({1, 0, 2}));
}


TEST(inorder, t3)
{
    ASSERT_EQ(binary_inorder(3),
              std::vector<int>({3, 1, 4, 0, 5, 2, 6}));
}


TEST(inorder, t4)
{
    ASSERT_EQ(binary_inorder(4),
              std::vector<int>(
                  {7, 3, 8, 1, 9, 4, 10, 0, 11, 5, 12, 2, 13, 6, 14}));
}
