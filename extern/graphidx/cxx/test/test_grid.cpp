#include <gtest/gtest.h>

#include "idx/grid.hpp"


TEST(GridIndex, grid_3x2)
{
    GridIndex g(3, 2);
}


TEST(GridIndex, binnotation)
{
    ASSERT_EQ(0b1001, 9);
    ASSERT_EQ(0b1000, 8);
    ASSERT_EQ(0b0000, 0);
}


TEST(GridIndex, kinds)
{
    ASSERT_EQ(ML, 2);
    ASSERT_EQ(LR, 9);
}


/*
  >>> np.array(range(6)).reshape(3, 2, order='F')
  array([[0, 3],
         [1, 4],
         [2, 5]])
*/
TEST(GridIndex, kind_3x2)
{
    GridIndex g {3, 2};
    ASSERT_EQ(g.kind(0 + 0*3), LL);
    ASSERT_EQ(g.kind(0 + 1*3), LR);
    ASSERT_EQ(g.kind(1 + 1*3), MR);
    ASSERT_EQ(g.kind(2 + 1*3), RR);
}


TEST(GridIndex, num_neighbors)
{
    ASSERT_EQ(num_neighbors(MM), 4);
    {
        const int k1 = 0b10;
        {
            const int k2 = 0;
            ASSERT_EQ(num_neighbors(GridPointKind(k1 << 2 | k2)), 3);
            ASSERT_EQ(num_neighbors(GridPointKind(k2 << 2 | k1)), 3);
        }
    }
    {
        const int k1 = 0b01;
        {
            const int k2 = 0;
            ASSERT_EQ(num_neighbors(GridPointKind(k1 << 2 | k2)), 3);
            ASSERT_EQ(num_neighbors(GridPointKind(k2 << 2 | k1)), 3);
        }
    }
}
