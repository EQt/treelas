#include <doctest/doctest.h>
#include <cstdint>

#include "../.tree_12.hpp"


TEST_CASE("Nodes: set_parent")
{
    approx::Nodes<> n;
    n.resize(2);
    n[0].set_parent(42, false);
    REQUIRE(!n[0].same());
    n[0].set_same(true);
    REQUIRE(n[0].same());
    n[0].set_same(false);
    REQUIRE(!n[0].same());
}


TEST_CASE("Nodes: int64")
{
    using namespace approx;

    const auto no = Node<float, int64_t>::one;
    const int64_t exp = (int64_t(1)<<63);
    REQUIRE(exp == no);
}


TEST_CASE("Nodes: extra")
{
    REQUIRE(0 < sizeof(std::array<char, 0>));
}



TEST_CASE("Nodes: empty_class")
{
    // https://stackoverflow.com/a/621648
    // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Empty_Base_Optimization
    using empty_t = struct Empty{};
    REQUIRE(1 == sizeof(empty_t));
}


TEST_CASE("Nodes: sizeof_node")
{
    REQUIRE(3*sizeof(float) + sizeof(int) == sizeof(approx::Node<float, int>));
}
