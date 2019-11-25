#include <doctest/doctest.h>
#include <vector>
#include <graphidx/utils/timer.hpp>          // TimerQuiet
#include <graphidx/bits/weights.hpp>

#include "../tree_dp.hpp"


TEST_CASE("tree_dp: two nodes")
{
    TimerQuiet _;
    const std::vector<int> parent = {0, 0};
    const std::vector<double> y = {1.0, 0.0};
    const int root = 0;
    const double lam = 0.2;
    auto x = tree_dp<>(y, parent, lam, root);
}
