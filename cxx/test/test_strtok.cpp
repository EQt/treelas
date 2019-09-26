/** understand strtok function */
#include <doctest/doctest.h>
#include <cstring>
#include <string>


TEST_CASE("Tokens: s1")
{
    const char *delim = "/";
    {
        char str[] = "a/bc/d";
        char *token = strtok(str, delim);
        REQUIRE("a" == std::string(token));
        token = strtok(nullptr, delim);
        REQUIRE("bc" == std::string(token));
        token = strtok(nullptr, delim);
        REQUIRE("d" == std::string(token));
        token = strtok(nullptr, delim);
        REQUIRE(nullptr == token);
    }
}


TEST_CASE("Tokens: empty_str")
{
    const char *delim = "/";
    char str[] = "";
    char *token = strtok(str, delim);
    REQUIRE(nullptr == token);
}


TEST_CASE("Tokens: empty")
{
    const char *delim = "/";
    {
        char str[] = "/";
        char *token = strtok(str, delim);
        REQUIRE(nullptr == token);
    }
}


TEST_CASE("Tokens: one")
{
    const char *delim = "/";
    {
        char str[] = "bla";
        char *token = strtok(str, delim);
        REQUIRE("bla" == std::string(token));
        token = strtok(nullptr, delim);
        INFO(token);
        REQUIRE(token == nullptr);
    }
}
