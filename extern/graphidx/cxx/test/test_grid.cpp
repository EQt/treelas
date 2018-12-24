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
        
