#include <doctest/doctest.h>
#include <vector>


/* Will the address sanatizer detect the following leak? */
TEST_CASE("memory leak")
{
    const std::vector<int> parent = {0, 0};
    REQUIRE(parent[2] == 3);
}
