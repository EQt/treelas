/*
  Useful resources
    https://github.com/zxqfl/flow/blob/master/bindings.cpp

  Alternative library?
    http://www.andres.sc/graph.html
 */
#include <vector>
#include <lemon/static_graph.h>
#include <lemon/preflow.h>


int
main()
{
    using Graph = lemon::StaticDigraph;
    using MaxFlow = lemon::Preflow<Graph, Graph::ArcMap<double>>;

    const size_t n = 5;
    Graph graph;
    Graph::ArcMap<double> capacities(graph);

    std::vector<std::pair<int,int>> arcs;
    arcs.push_back(std::make_pair(0,1));
    arcs.push_back(std::make_pair(0,2));
    arcs.push_back(std::make_pair(1,3));
    arcs.push_back(std::make_pair(1,2));
    arcs.push_back(std::make_pair(3,0));

    graph.build(n, arcs.begin(), arcs.end());

    MaxFlow mf (graph, capacities, graph.nodeFromId(0), graph.nodeFromId(1));
    mf.runMinCut();

    std::vector<bool> above (n, false);
    for (int i = 0; i < (int)n; i++)
        above[i] = mf.minCut(graph.nodeFromId(i));

    return 0;
}
