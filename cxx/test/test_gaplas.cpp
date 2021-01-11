#include <doctest/doctest.h>

#include <graphidx/bits/weights.hpp>
#include <graphidx/edges.hpp>
#include <graphidx/grid.hpp>
#include <graphidx/idx/incidence.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/lemon_heap.hpp>
#include <graphidx/utils/timer.hpp>

#include "../gaplas.hpp"
#include "demo3x7.hpp"


TEST_CASE("gap: demo3x7")
{
    TimerQuiet _;
    GridGraph graph {3, 7};
    IncidenceIndex<int> idx {graph};
    const size_t m = graph.num_edges(), n = graph.num_nodes();
    REQUIRE(m == 32);
    REQUIRE(n == 21);
    const auto lam = Ones<double>();
    const auto mu = Ones<double>();
    std::vector<double> y {
        (double *)demo_3x7_y[0], (double *)demo_3x7_y[0] + graph.num_nodes()};
    std::vector<double> x(y.size(), 0.0);
    REQUIRE(y.size() == n);
    REQUIRE(x.size() == n);
    const int root = 0;

    GapMem<double> mem(x.data(), y.data(), n, m, root);
    mem.init();
    {
        const auto lam = Const<double>(1);
        mem.gap_vec(idx, lam);
    }
    const static double gap0[32] = {
        -0.0,  -1.0,  -0.24, -0.04, -0.43, -0.73, -0.7,  -0.28, -1.41, -1.43, -0.51,
        -0.52, -1.14, -0.02, -0.11, -0.02, -0.79, -0.33, -0.74, -0.65, -1.11, -0.78,
        -0.51, -0.14, -0.44, -0.78, -0.58, -0.80, -1.43, -0.13, -1.06, -0.38};
    for (size_t e = 0; e < m; e++) {
        CAPTURE(e);
        CHECK(doctest::Approx(gap0[e]) == mem.gamma[e]);
    }
    auto tlam = lam;
    {
        using Queue = QuadHeapT;
        const std::vector<int> expect = {0, 4, 5,  0,  3,  4,  7,  8,  5,  6, 7,
                                         8, 9, 14, 17, 12, 15, 16, 19, 16, 17};
        mem.template find_tree<Queue>(idx, tlam, lam);
        REQUIRE(find_root(mem.parent) == root);
        REQUIRE(mem.parent == expect);
    }
    {
        mem.tree_opt(tlam, mu);
        const auto expect = (const double *) demo_3x7_x1[0];
        for (size_t v = 0; v < n; v++) {
            CAPTURE(v);
            CHECK(doctest::Approx(mem.x[v]) == expect[v]);
        }
    }
    {
        mem.update_duals(idx);
    }
}
