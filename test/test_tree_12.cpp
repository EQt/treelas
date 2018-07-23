#include <gtest/gtest.h>
#include <cstdint>

#include "../cxx/tree_12.hpp"


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
    struct Empty{} e;
    ASSERT_EQ(sizeof(e), 1);
}


TEST(Nodes, sizeof_node)
{
    ASSERT_EQ(sizeof(approx::Node<float, int>), 3*sizeof(float) + sizeof(int));
}



// TEST(Nodes, init)
// {
//     const std::vector<int>
//         parent {0, 0, 1, 0, 1, 2},
//         order  {3, 4, 5, 2, 1, 0},
//         iordex {5, 4, 3, 0, 1, 2};
//     const int n = parent.size();

//     const auto iorder = iperm(order);
//     ASSERT_EQ(iorder, iordex);

//     approx::Nodes<double, int> nodes;
//     nodes.resize(n);
//     ASSERT_EQ(nodes.ny.size(), n);
//     ASSERT_EQ(nodes.nx.size(), n);
//     ASSERT_EQ(nodes.deriv.size(), n);

//     const std::vector<double> y {0, 1, 2, 3, 4, 5};
//     ASSERT_EQ(y.size(), n);

//     std::cout << "postorder=" << order;

//     const double x0 = -42;
//     init(nodes, parent, order, iorder, y, x0);
//     for (int i = 0; i < n; i++) {
//         const auto ii = order[i];
//         ASSERT_EQ(-nodes.ny[i], double(ii))
//             << "i=" << i << "; ii=" << ii << std::endl;
//         ASSERT_EQ(nodes.nx[i], x0) << "i=" << i;
//         ASSERT_EQ(nodes.parent(i), parent[ii])
//             << "i=" << i << "; ii=" << ii << std::endl;
//     }
// }
