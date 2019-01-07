#include <gtest/gtest.h>
#include "../utils/timer.hpp"


TEST(Timer, endsnewline)
{
    ASSERT_FALSE(endsnewline(""));
    ASSERT_FALSE(endsnewline("asdf"));
    ASSERT_FALSE(endsnewline("a"));
    ASSERT_TRUE(endsnewline("a\n"));
    ASSERT_FALSE(endsnewline("a\na"));
    ASSERT_TRUE(endsnewline("a\na\n"));
}
