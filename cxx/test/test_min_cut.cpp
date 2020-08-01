#ifdef HAVE_LEMON
#include <doctest/doctest.h>
#include <iostream>
#include <set>

#include "lemon_throws.hpp"
#include "../min_cut.hpp"


std::set<size_t>
to_set(const std::vector<bool> &v)
{
    std::set<size_t> s;
    for (size_t i = 0; i < v.size(); i++)
        if (v[i])
            s.insert(i);
    return s;
}


TEST_CASE("min_cut")
{
    SUBCASE("wiki1")
    {
        /*
            ->(3)\
           /10 |   \5
          (0)  |15 >v(2)
            \5 v  /10
             \>(1)

          https://en.wikipedia.org/wiki/Maximum_flow_problem#/media/File:MFP1.jpg
        */

        const size_t n = 4;
        std::vector<std::pair<int,int>> arcs {
            {0, 1}, // 5
            {0, 3}, // 10
            {1, 2}, // 10
            {3, 1}, // 15
            {3, 2}, // 5
        };
        double capacities[] = {5, 10, 10, 15, 5};
        auto above = min_cut(n, arcs, capacities, 0, 2);
        REQUIRE(to_set(above) == std::set<size_t>({0}));
    }

    SUBCASE("chengw1005")
    {
        // http://chengw1005.blogspot.com/2015/11/graph-network-flow-problems.html
        std::vector<std::tuple<int, int, double>>  warcs = {
            {0, 2, 13},
            {0, 1, 16},
            {1, 2, 10},
            {1, 3, 12},
            {2, 1, 4},
            {2, 4, 14},
            {3, 2, 9},
            {3, 5, 20},
            {4, 3, 7},
            {4, 5, 4}
        };
        const int source = 0, target = 5;
        const size_t n = 6;
        const auto above = min_cut(n, warcs, source, target);
        REQUIRE(to_set(above) == std::set<size_t>({0, 1, 2, 4}));
    }
    SUBCASE("chengw1005:permuted")
    {
        // http://chengw1005.blogspot.com/2015/11/graph-network-flow-problems.html
        std::vector<std::tuple<int, int, double>>  warcs = {
            {0, 2, 13},
            {2, 4, 14},
            {1, 2, 10},
            {1, 3, 12},
            {0, 1, 16},
            {2, 1, 4},
            {3, 2, 9},
            {3, 5, 20},
            {4, 3, 7},
            {4, 5, 4}
        };
        const int source = 0, target = 5;
        const size_t n = 6;
        REQUIRE_THROWS_AS(min_cut(n, warcs, source, target), lemon_assert);
    }
}
#endif
