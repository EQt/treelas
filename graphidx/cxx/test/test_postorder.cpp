#include <gtest/gtest.h>
#include "idx/utils/perm.hpp"
#include "tree/postorder.hpp"

#include "../../cxx/utils/timer.hpp"


TEST(postorder, size)
{
    TimerQuiet _;
    const std::vector<int> parent = {0, 0, 1, 2, 3, 0, 7, 8, 3, 8};
    PostOrder pord (parent);
    ASSERT_EQ(parent.size(), 10);
    ASSERT_EQ(pord.size(), parent.size()-1);
}


TEST(postorder, size_with_root)
{
    TimerQuiet _;
    const std::vector<int> parent = {0, 0, 1, 2, 3, 0, 7, 8, 3, 8};
    int *postord = nullptr;
    const int root = 0;
    const bool include_root = true;
    PostOrder pord (parent, postord, root, include_root);
    ASSERT_EQ(parent.size(), 10);
    ASSERT_EQ(pord.size(), parent.size());
}


TEST(postorder, is_perm)
{
    TimerQuiet _;
    const std::vector<int> parent = {0, 0, 1, 2, 3, 0, 7, 8, 3, 8};
    int *postord = nullptr;
    const int root = 0;
    const bool include_root = true;
    PostOrder pord (parent, postord, root, include_root);
    std::vector<int> vpord (pord.begin(), pord.end());
    ASSERT_TRUE(is_perm_mut(vpord));
}


TEST(postorder, tree_mini)
{
    TimerQuiet _;
    const std::vector<int> parent {0, 0, 1, 0, 1, 2};
    const auto n = parent.size();
    const int root = 0;

    std::vector<int> postord (n, -1);
    post_order(n, parent.data(), root, postord.data());

    const std::vector<int> expected_ord { 3, 4, 5, 2, 1, 0 };
    ASSERT_EQ(postord, expected_ord);
}
