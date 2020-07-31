/*
  Useful resources
    https://gist.github.com/huanyud/45f98d8bf8d6df66d3e7ab3e9a85af90
    https://github.com/zxqfl/flow/blob/master/bindings.cpp

  Alternative library?
    http://www.andres.sc/graph.html
 */
#include <iostream>
#include <tuple>
#include <vector>
#include <lemon/static_graph.h>
#include <lemon/preflow.h>


template <typename Arcs, typename float_t = double>
std::vector<bool>
min_cut(
    const size_t n,
    const Arcs arcs,
    const float_t *capacities,
    const int source,
    const int target)
{
    using Graph = lemon::StaticDigraph;
    using MaxFlow = lemon::Preflow<Graph, Graph::ArcMap<float_t>>;

    const auto m = arcs.size();
    Graph graph;
    graph.build(n, arcs.begin(), arcs.end());
    Graph::ArcMap<double> capacity(graph);
    for (int i = 0; i < int(m); i++)
        capacity[graph.arcFromId(i)] = capacities[i];

    MaxFlow mf (graph, capacity, graph.nodeFromId(source), graph.nodeFromId(target));
    mf.runMinCut();

    std::vector<bool> above (n, false);
    for (int i = 0; i < (int)n; i++)
        above[i] = mf.minCut(graph.nodeFromId(i));
    return above;
}


void
print_min_cut(const std::vector<bool> &above)
{
    std::cout << "min_cut = {";
    for (size_t i = 0; i < above.size(); i++)
        if (above[i]) {
            std::cout << " " << i << ",";
        }
    std::cout << "}" << std::endl;
}


void
test_example1()
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
    print_min_cut(above);
}


void
test_example2()
{
    // http://chengw1005.blogspot.com/2015/11/graph-network-flow-problems.html
    std::vector<std::tuple<int, int, double>> warcs {
        {0, 2, 13},
        {0, 1, 16},
        {1, 2, 10},
        {1, 3, 12},
        {2, 1, 4},
        {2, 4, 14},
        {3, 2, 9},
        {3, 5, 20},
        {4, 3, 7},
        {4, 5, 4}};

    std::vector<std::pair<int, int>> arcs;
    std::vector<double> cap;
    for (auto &wa: warcs) {
        arcs.push_back(std::make_pair(std::get<0>(wa), std::get<1>(wa)));
        cap.push_back(std::get<2>(wa));
    }
    const int source = 0, target = 5;
    const size_t n = 6;
    auto above = min_cut(n, arcs, cap.data(), source, target);
    print_min_cut(above);
}


int
main()
{
    // test_example1();
    test_example2();
    return 0;
}

// Local Variables:
// compile-command: "cd ../../build && COLOR=0 make  maxflow && ./maxflow"
// End:
