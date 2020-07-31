/*
  Useful resources
    https://gist.github.com/huanyud/45f98d8bf8d6df66d3e7ab3e9a85af90
    https://github.com/zxqfl/flow/blob/master/bindings.cpp

  Alternative library?
    http://www.andres.sc/graph.html
 */
#include <iostream>
#include <vector>
#include <lemon/static_graph.h>
#include <lemon/preflow.h>


int
main()
{
    /*
        ->(3)\
       /10 |   \5
      (0)  |15 >v(2)
        \5 v  /10
         \>(1)

      https://en.wikipedia.org/wiki/Maximum_flow_problem#/media/File:MFP1.jpg
     */
    using Graph = lemon::StaticDigraph;
    using MaxFlow = lemon::Preflow<Graph, Graph::ArcMap<double>>;

    const size_t n = 4;

    Graph graph;
    std::vector<std::pair<int,int>> arcs {
        {0, 1}, // 5
        {0, 3}, // 10
        {1, 2}, // 10
        {3, 1}, // 15
        {3, 2}, // 5
    };
    graph.build(n, arcs.begin(), arcs.end());

    Graph::ArcMap<double> capacity(graph);
    {
        int i = 0;
        for (auto c : {5, 10, 10, 15, 5})
            capacity[graph.arcFromId(i++)] = double(c);
    }

    MaxFlow mf (graph, capacity, graph.nodeFromId(0), graph.nodeFromId(1));
    mf.runMinCut();

    std::vector<bool> above (n, false);
    for (int i = 0; i < (int)n; i++)
        above[i] = mf.minCut(graph.nodeFromId(i));

    std::cout << "above = {";
    for (int i = 0; i < (int)n; i++)
        if (above[i]) {
            std::cout << " " << i << ",";
        }
    std::cout << "}" << std::endl;
    return 0;
}
