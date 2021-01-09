#include <algorithm> // for std::equal
#include <doctest/doctest.h>

#include <graphidx/bits/weights.hpp>
#include <graphidx/grid.hpp>

#include "../gaplas.hpp"
#include "demo3x7.hpp"
#include "graphidx/idx/incidence.hpp"


TEST_CASE("gap: demo3x7")
{
    GridGraph graph(3, 7);
    IncidenceIndex<int> idx {graph};
    const size_t m = graph.num_edges();
    CHECK(7 * 2 + 6 * 3 == 32);
    CHECK(m == 32);
    const static double gap0[32] = {
        -0.0,  -1.0,  -0.24, -0.04, -0.43, -0.73, -0.7,  -0.28, -1.41, -1.43, -0.51,
        -0.52, -1.14, -0.02, -0.11, -0.02, -0.79, -0.33, -0.74, -0.65, -1.11, -0.78,
        -0.51, -0.14, -0.44, -0.78, -0.58, -0.80, -1.43, -0.13, -1.06, -0.38};
    std::vector<double> y {
        (double *)demo_3x7_y[0], (double *)demo_3x7_y[0] + graph.num_nodes()};
    std::vector<double> x(y.size());
    const int root = 0;

    GapMem<double> mem {x.data(), y.data(), graph.num_nodes(), graph.num_edges(), root};
    mem.init();
    {
        const auto lam = Ones<double>();
        mem.gap_vec(idx, lam);
        CHECK(std::vector<double>(mem.gamma) == std::vector<double>(gap0, gap0 + m));
    }
}
