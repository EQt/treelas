#include <gtest/gtest.h>
#include "../line/deque.hpp"


TEST(DeQue, empty_bool)
{
    DeQue<int> q;
    ASSERT_FALSE(q);
    ASSERT_EQ(q.size(), 0);
}
