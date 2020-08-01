/*
  Useful resources
    https://gist.github.com/huanyud/45f98d8bf8d6df66d3e7ab3e9a85af90
    https://github.com/zxqfl/flow/blob/master/bindings.cpp

  Alternative library?
    http://www.andres.sc/graph.html
 */
#ifdef HAVE_LEMON

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

#endif
