/** understand strtok function */
#include <gtest/gtest.h>
#include <cstring>
#include <string>


TEST(Tokens, s1)
{
    const char *delim = "/";
    {
        char str[] = "a/bc/d";
        char *token = strtok(str, delim);
        ASSERT_EQ(std::string(token), "a");
        token = strtok(nullptr, delim);
        ASSERT_EQ(std::string(token), "bc");
        token = strtok(nullptr, delim);
        ASSERT_EQ(std::string(token), "d");
        token = strtok(nullptr, delim);
        ASSERT_EQ(token, nullptr);
    }
}


TEST(Tokens, empty_str)
{
    const char *delim = "/";
    char str[] = "";
    char *token = strtok(str, delim);
    ASSERT_EQ(token, nullptr);
}


TEST(Tokens, empty)
{
    const char *delim = "/";
    {
        char str[] = "/";
        char *token = strtok(str, delim);
        ASSERT_EQ(token, nullptr);
    }
}


TEST(Tokens, one)
{
    const char *delim = "/";
    {
        char str[] = "bla";
        char *token = strtok(str, delim);
        ASSERT_EQ(std::string(token), "bla");
        token = strtok(nullptr, delim);
        ASSERT_EQ(token, nullptr) << std::string(token);
    }
}
