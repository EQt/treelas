#include <gtest/gtest.h>
#include "../idx/biadjacent.hpp"


TEST(biadj, unequal_len)
{
    const std::vector<int>
        head {0, 1, 2, 3},
        tail {1, 2, 3, 0, 5};
    ASSERT_THROW(BiAdjacent(head, tail), std::invalid_argument);
}


TEST(biadj, cell4)
{
    const std::vector<int>
        head {0, 1, 2, 3},
        tail {1, 2, 3, 0};

    const int n = 4;
    BiAdjacent b (head, tail);
    for (int i = 0; i < n; i++)
        EXPECT_EQ(std::set<int>(b[i]),
                  std::set<int>({(i+n-1)%n, (i+n+1)%n})) << i;
}

// Local Variables:
// compile-command: "cd ../build && make tests && ./tests --gtest_filter='biadj*'"
// End:
