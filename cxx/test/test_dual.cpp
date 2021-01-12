#include <doctest/doctest.h>

#include "../gaplas.hpp"


TEST_CASE("dual dual 3")
{
    constexpr size_t n = 3;
    const int post_order[n-1] = {1, 2};
    const int parent[n] = {0, 0, 0};
    const double x[n] = {-3, 1, 2};
    const double y[n] = {0};
    double alpha[n] = {0};
    tree_dual(n, alpha, x, y, parent, post_order);
    REQUIRE(doctest::Approx(alpha[0]) == 0);
    REQUIRE(doctest::Approx(alpha[1]) == 1);
    REQUIRE(doctest::Approx(alpha[2]) == 2);
}
