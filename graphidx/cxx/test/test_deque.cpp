#include <gtest/gtest.h>
#include "../std/deque.hpp"


TEST(DeQue, empty_bool)
{
    DeQue<int> q;
    ASSERT_FALSE(q);
    ASSERT_EQ(q.size(), 0);
}


TEST(DeQue, one_push)
{
    DeQue<int> q (1);
    ASSERT_FALSE(q);
    ASSERT_EQ(q.size(), 0);

    q.push<false>(42);
    ASSERT_TRUE(q);
    ASSERT_EQ(q.size(), 1);
    ASSERT_EQ(q.peek<true>(), 42);
    ASSERT_EQ(q.peek<false>(), 42);
}


TEST(DeQue, fbf)
{
    DeQue<int> q (2);
    q.push<true>(1);
    q.push<false>(5);
    q.push(2);

    ASSERT_TRUE(q);
    ASSERT_EQ(q.size(), 3);
    ASSERT_EQ(q.peek<true>(), 2);
    ASSERT_EQ(q.peek<false>(), 5);
}
