#include <gtest/gtest.h>
#include "../spanning/kruskal_mst.hpp"


TEST(KruskalMST, square1)
{
    const std::vector<int>
        head {0, 0, 1, 2},
        tail {2, 1, 3, 3};

    const size_t n = 4;

    {
        const std::vector<double> weights = {0, 0, 0, 1};

        const auto included = minimum_spanning_tree(n, head, tail, weights);
        ASSERT_EQ(included, std::vector<bool>({true, true, true, false}));
    }
    {
        const std::vector<double> weights = {0, 0, 1, 0};

        const auto included = minimum_spanning_tree(n, head, tail, weights);
        ASSERT_EQ(included, std::vector<bool>({true, true, false, true}));
    }
    {
        const std::vector<double> weights = {1, 0, 0, 0};

        const auto included = minimum_spanning_tree(n, head, tail, weights);
        ASSERT_EQ(included, std::vector<bool>({false, true, true, true}));
    }
}
