#include <doctest/doctest.h>
#include <cstdint>
#include <array>

#include "../tree_apx.hpp"


TEST_CASE("TreeApx: init_parent")
{
    TreeApx<> tapx (2, nullptr);
    tapx.init_parent(1, 0);
    REQUIRE(tapx.parent(1) == 0);
    REQUIRE(tapx.same(1));
    tapx.divorce(1);
    REQUIRE(!tapx.same(1));
}


TEST_CASE("TreeApx: int64::one")
{
    const auto no = TreeApx<float, int64_t>::one;
    const int64_t exp = (int64_t(1)<<63);
    REQUIRE(exp == no);
}


TEST_CASE("Nodes: extra")
{
    REQUIRE(0 < sizeof(std::array<char, 0>));
}


// https://stackoverflow.com/a/621648
// https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Empty_Base_Optimization
TEST_CASE("Nodes: empty_class")
{
    using empty_t = struct Empty{};
    REQUIRE(1 == sizeof(empty_t));
}
