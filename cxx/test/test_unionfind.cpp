#include <gtest/gtest.h>
#include "../spanning/unionfind.hpp"


TEST(UnionFind, mini_test_2)
{
    UnionFind<int> u (2);
    ASSERT_EQ(u.find(1), 1);
    ASSERT_EQ(u.find(0), 0);

    u.unite(u.find(1), u.find(0));
    ASSERT_EQ(u.find(0), u.find(1));
    ASSERT_EQ(u.find(1), u.find(0));
}


TEST(UnionFind, mini_test_3)
{
    UnionFind<int> u (3);
    ASSERT_EQ(u.find(2), 2);
    ASSERT_EQ(u.find(1), 1);
    ASSERT_EQ(u.find(0), 0);

    u.unite(u.find(2), u.find(0));
    EXPECT_EQ(u.find(0), u.find(2));
    EXPECT_EQ(u.find(2), u.find(0));

    EXPECT_NE(u.find(2), u.find(1));
    EXPECT_NE(u.find(1), u.find(0));

    u.unite(u.find(1), u.find(0));
    EXPECT_EQ(u.find(2), u.find(1));
    EXPECT_EQ(u.find(1), u.find(0));
    EXPECT_EQ(u.find(0), u.find(1));
}


TEST(UnionFind, mini_test_4)
{
    UnionFind<int> u (4);
    ASSERT_EQ(u.find(3), 3);
    ASSERT_EQ(u.find(2), 2);
    ASSERT_EQ(u.find(1), 1);
    ASSERT_EQ(u.find(0), 0);

    u.unite(u.find(3), u.find(0));
    EXPECT_EQ(u.find(3), u.find(0));
    EXPECT_EQ(u.find(3), u.find(0));
    EXPECT_EQ(u.find(0), u.find(0));
    EXPECT_EQ(u.find(0), u.find(3));

    EXPECT_NE(u.find(3), u.find(2));
    EXPECT_NE(u.find(2), u.find(3));
    EXPECT_NE(u.find(2), u.find(0));
    EXPECT_NE(u.find(2), u.find(0));

    u.unite(u.find(3), u.find(2));
    EXPECT_EQ(u.find(3), u.find(2));
    EXPECT_EQ(u.find(2), u.find(3));
    EXPECT_EQ(u.find(2), u.find(0));
    EXPECT_EQ(u.find(2), u.find(0));
    EXPECT_EQ(u.find(0), u.find(2));
}


TEST(UnionFind, mini_test_4a)
{
    UnionFind<int> u (4);
    u.unite(u.find(3), u.find(0));

    EXPECT_NE(u.find(2), u.find(3));

    u.unite(u.find(2), u.find(0));

    EXPECT_EQ(u.find(2), u.find(3));
    EXPECT_NE(u.find(2), u.find(1));
    EXPECT_NE(u.find(3), u.find(1));

    u.unite(u.find(0), u.find(1));

    EXPECT_EQ(u.find(2), u.find(1));
    EXPECT_EQ(u.find(3), u.find(1));
    EXPECT_EQ(u.find(2), u.find(0));
    EXPECT_EQ(u.find(3), u.find(0));
}
