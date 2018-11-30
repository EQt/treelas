#include <gtest/gtest.h>
#include <cstdint>

#include "../tree_12.hpp"


TEST(Nodes, set_parent)
{
    approx::Nodes<> n;
    n.resize(2);
    n[0].set_parent(42, false);
    ASSERT_FALSE(n[0].same());
    n[0].set_same(true);
    ASSERT_TRUE(n[0].same());
    n[0].set_same(false);
    ASSERT_FALSE(n[0].same());
}


TEST(Nodes, int64)
{
    using namespace approx;

    const auto no = Node<float, int64_t>::one;
    const int64_t exp = (int64_t(1)<<63);
    ASSERT_EQ(no, exp);
}


TEST(Nodes, extra)
{
    ASSERT_GT(sizeof(std::array<char, 0>), 0);
}



TEST(Nodes, empty_class)
{
    // https://stackoverflow.com/a/621648
    // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Empty_Base_Optimization
    using empty_t = struct Empty{};
    ASSERT_EQ(sizeof(empty_t), 1);
}


TEST(Nodes, sizeof_node)
{
    ASSERT_EQ(sizeof(approx::Node<float, int>), 3*sizeof(float) + sizeof(int));
}
