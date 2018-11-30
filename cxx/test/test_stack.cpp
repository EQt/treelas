#include <gtest/gtest.h>
#include "../stack.hpp"
#if !_STACK_DEBUG
#  error "Need to compile stack.hpp with #define _STACK_DEBUG 1"
#endif


TEST(stack, debugging)
{
    #ifndef _STACK_DEBUG
    ASSERT_TRUE(false);
    #endif
    ASSERT_TRUE(_STACK_DEBUG);
    ASSERT_TRUE(stack<int>::debug_mode());
    // ASSERT_EQ(sizeof(stack<int>), sizeof(int*) + sizeof(int) + sizeof(int));
}


TEST(stack, nothing)
{
    stack<int> stack;
}


TEST(stack, nothing_dbg)
{
    stack<int> stack;
}


TEST(stack, just_allocate)
{
    stack<int> stack;
    ASSERT_TRUE(stack.empty());
    ASSERT_EQ(stack.size(), 0);
    stack.reserve(1);
    ASSERT_EQ(stack.size(), 0);
}


TEST(stack, underflow0)
{
    stack<int> s;
    ASSERT_EQ(s.size(), 0);
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.pos, 0);
    ASSERT_THROW(s.back(), std::underflow_error);
    ASSERT_THROW(s.pop_back(), std::underflow_error);
}


TEST(stack, push1)
{
    stack<int> stack;
    ASSERT_TRUE(stack.empty());
    ASSERT_EQ(stack.size(), 0);
    stack.reserve(1);
    ASSERT_EQ(stack.size(), 0);
    ASSERT_EQ(stack.capacity(), 1);
    stack.push_back(-42);
    ASSERT_EQ(stack.size(), 1);
    ASSERT_EQ(stack.back(), -42);
}


TEST(stack, throws)
{
    stack<int> stack;
    ASSERT_TRUE(stack.empty());
    ASSERT_EQ(stack.size(), 0);
    ASSERT_THROW(stack.back(), std::underflow_error);
    ASSERT_EQ(stack.size(), 0);
    ASSERT_EQ(stack.capacity(), 0);
    ASSERT_THROW(stack.reserve(0), std::invalid_argument);
    stack.reserve(2);
    ASSERT_EQ(stack.capacity(), 2);
    ASSERT_EQ(stack.size(), 0);
    stack.push_back(1);
    ASSERT_EQ(stack.size(), 1);
    stack.push_back(2);
    ASSERT_EQ(stack.size(), 2);
    ASSERT_EQ(stack.capacity(), 2);
    ASSERT_THROW(stack.push_back(3), std::overflow_error);
}
