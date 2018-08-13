#include <gtest/gtest.h>
#include "../cxx/queue.hpp"

TEST(queue, nothing)
{
    queue<int> queue;
    ASSERT_EQ(queue.capacity(), 0);
    ASSERT_EQ(queue.size(), 0);
}


TEST(queue, just_allocate)
{
    queue<int> queue;
    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(queue.size(), 0);
    queue.reserve(1);
    ASSERT_EQ(queue.size(), 0);
    ASSERT_EQ(queue.capacity(), 1);
}


TEST(queue, push1)
{
    queue<int> queue;
    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(queue.size(), 0);
    queue.reserve(1);
    ASSERT_EQ(queue.size(), 0);
    ASSERT_EQ(queue.capacity(), 1);
    queue.push(-42);
    ASSERT_EQ(queue.size(), 1);
    ASSERT_EQ(queue.front(), -42);
}


TEST(queue, push2)
{
    queue<int> queue;
    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(queue.size(), 0);
    ASSERT_EQ(queue.capacity(), 0);

    queue.reserve(2);
    ASSERT_EQ(queue.capacity(), 2);
    ASSERT_EQ(queue.size(), 0);

    queue.push(1);
    ASSERT_EQ(queue.size(), 1);
    queue.push(2);
    ASSERT_EQ(queue.size(), 2);
    ASSERT_EQ(queue.capacity(), 2);

    ASSERT_EQ(queue.front(), 1);
    queue.pop();
    ASSERT_EQ(queue.size(), 1);
    ASSERT_EQ(queue.front(), 2);

    queue.push(3);
    ASSERT_EQ(queue.size(), 2);
    queue.pop();
    ASSERT_EQ(queue.size(), 1);
    queue.pop();
    ASSERT_EQ(queue.size(), 0);
    ASSERT_TRUE(queue.empty());
}
