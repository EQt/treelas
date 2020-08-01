/*
  Useful resources
    https://gist.github.com/huanyud/45f98d8bf8d6df66d3e7ab3e9a85af90
    https://github.com/zxqfl/flow/blob/master/bindings.cpp

  Alternative library?
    http://www.andres.sc/graph.html
 */
#ifdef HAVE_LEMON

#include <tuple>
#include <vector>

#include <lemon/static_graph.h>
#include <lemon/preflow.h>


template <typename int_t = int>
using IArc = std::pair<int_t, int_t>;


template <typename int_t = int, typename float_t = double>
struct WeightedArc
{
    int_t head, tail;
    float_t weight;
};


template <typename bool_ptr, typename int_t = int, typename float_t = double>
bool_ptr
min_cut(
    bool_ptr &above,
    const size_t n,
    const size_t m,
    const IArc<int_t> *arcs,
    const float_t *capacities,
    const int source,
    const int target)
{
    using Graph = lemon::StaticDigraph;
    using MaxFlow = lemon::Preflow<Graph, Graph::ArcMap<float_t>>;

    Graph graph;
    graph.build(n, arcs, arcs + m);
    Graph::ArcMap<double> capacity(graph);
    for (int i = 0; i < int(m); i++)
        capacity[graph.arcFromId(i)] = capacities[i];

    MaxFlow mf (graph, capacity, graph.nodeFromId(source), graph.nodeFromId(target));
    mf.runMinCut();

    for (int i = 0; i < (int)n; i++)
        above[i] = mf.minCut(graph.nodeFromId(i));
    return above;
}


template <typename int_t = int, typename float_t = double>
std::vector<bool>
min_cut(
    const size_t n,
    const std::vector<IArc<int_t>> &arcs,
    const float_t *capacities,
    const int source,
    const int target)
{
    std::vector<bool> cut (n);
    return min_cut(cut, n, arcs.size(), arcs.data(), capacities, source, target);
}


template <typename WArcs, typename float_t = double>
std::vector<bool>
min_cut(
    const size_t n,
    const WArcs &warcs,
    const int source,
    const int target)
{
    std::vector<IArc<int>> arcs;
    std::vector<float_t> cap;
    for (auto &wa: warcs) {
        arcs.push_back({std::get<0>(wa), std::get<1>(wa)});
        cap.push_back(std::get<2>(wa));
    }
    return min_cut(n, arcs, cap.data(), source, target);
}


#endif
