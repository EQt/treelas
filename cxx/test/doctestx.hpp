#pragma once
#include <doctest/doctest.h>
#include <string>
#include <tuple>


namespace doctest {

using T = std::tuple<int, int>;
template <>
struct StringMaker<T>
{
    static String convert(const T &v)
    {
        return toString(
            "[" + std::to_string(std::get<0>(v)) + ", " +
            std::to_string(std::get<1>(v)) + "]");
    }
};

} // namespace doctest
