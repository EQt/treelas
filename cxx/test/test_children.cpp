#include <gtest/gtest.h>
#include <set>
#include "../cxx/children.hpp"
#include "../cxx/utils/perm.hpp"


TEST(children, generate5)
{
    // generate 5

    // ┌───┐
    // │ 5 │
    // └───┘
    //   │
    //   │
    //   ▼
    // ┌───┐
    // │ 0 │
    // └───┘
    //   ▲
    //   │
    //   │
    // ┌───┐
    // │ 1 │
    // └───┘
    //   ▲
    //   │
    //   │
    // ┌───┐
    // │ 2 │
    // └───┘
    //   ▲
    //   │
    //   │
    // ┌───┐
    // │ 3 │ ◀─────┐
    // └───┘       │
    //   ▲         │
    //   │         │
    //   │         │
    // ┌───┐       │
    // │ 4 │       │
    // └───┘       │
    //             │
    //             │
    //             │
    // ┌───┐     ┌───┐
    // │ 9 │ ──▶ │ 8 │
    // └───┘     └───┘
    //             ▲
    //             │
    //             │
    // ┌───┐     ┌───┐
    // │ 6 │ ──▶ │ 7 │
    // └───┘     └───┘
    const std::vector<int> parent = {0, 0, 1, 2, 3, 0, 7, 8, 3, 8};
    const int n = 10;
    EXPECT_EQ(parent.size(), n);

    std::vector<std::set<int>> cidx_expect (n);
    cidx_expect[0] = {1, 5};
    cidx_expect[1] = {2};
    cidx_expect[2] = {3};
    cidx_expect[3] = {4, 8};
    cidx_expect[8] = {7, 9};
    cidx_expect[7] = {6};

    ChildrenIndex cidx(parent);
    if (false) {
        printf("children[3]:\n");
        for (const auto c : cidx[3]) {
            printf("c = %d\n", c);
        }
    }
    for (int i = 0; i < n; i++) {
        std::set<int> children(cidx[i]);
        EXPECT_EQ(children, cidx_expect[i]) << "i = " << i;
    }
}


TEST(children, binary)
{
    const std::vector<int> parent = {0, 0, 0, 1, 1, 2, 2};
    const ChildrenIndex cidx (parent);
    for (int i : {0, 1, 2}) {
        EXPECT_EQ(cidx[i].size(), 2) << i;
    }
    for (int i = 3; i < (int) parent.size(); i++) {
        EXPECT_EQ(cidx[i].size(), 0) << i;
    }
}


TEST(children, root1)
{
    const std::vector<int> parent =
        {6,  6,  6,  7,  7,  7,  8,  8,  8};
    const int root = 8;
    ASSERT_EQ(parent.size(), 9);
    ASSERT_EQ(root, parent[root]);
    ChildrenIndex cidx (parent, root);

    const std::vector<int>
        idx_x ({0, 0, 0, 0, 0, 0, 0, 3, 6, 8}),
        value_x  ({0, 1, 2, 3, 4, 5, 6, 7, 8});

    ASSERT_EQ(cidx.index, idx_x);
    ASSERT_EQ(cidx.value, value_x);
}


TEST(children, root2)
{
    const std::vector<int> parent =
    //   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
        {6,  6,  6,  7,  7,  7,  8,  8, 17, 15, 15, 15, 16, 16, 16, 17, 17, 17};
    const int root = 17;
    ASSERT_EQ(parent.size(), 18);
    ASSERT_EQ(root, parent[root]);
    ChildrenIndex cidx (parent, root);

    ASSERT_TRUE(is_perm(cidx.value));

    const int c = 9;
    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(cidx[i+0].size(), 0);
        EXPECT_EQ(std::set<int>(cidx[i+c]), std::set<int>()) << i+c;
    }
    EXPECT_EQ(cidx[6+0], std::set<int>({0+0, 1+0, 2+0}));
    EXPECT_EQ(cidx[6+c], std::set<int>({0+c, 1+c, 2+c}));
    EXPECT_EQ(cidx[7+0], std::set<int>({3+0, 4+0, 5+0}));
    EXPECT_EQ(cidx[7+c], std::set<int>({3+c, 4+c, 5+c}));
    EXPECT_EQ(cidx[8+0], std::set<int>({6+0, 7+0}));
    EXPECT_EQ(cidx[8+c], std::set<int>({6+c, 7+c, 8}));
}
