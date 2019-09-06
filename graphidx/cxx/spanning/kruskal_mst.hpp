#pragma once

#include <vector>
#include <algorithm>        // for std::random_shuffle, std::sort
#include <random>

#include "unionfind.hpp"


std::vector<bool>
_spanning_tree(const size_t n,
               const size_t m,
               const std::vector<int> &source,
               const std::vector<int> &target,
               const std::vector<int> &edges)
{
    std::vector<bool> included(m, false);
    UnionFind<int> uf (n);
    for (const auto e : edges) {
        const int
            u = source[e],
            v = target[e];
        const auto
            fu = uf.find(u),
            fv = uf.find(v);
        if (fu != fv) {
            included[e] = true;
            uf.unite(fu, fv);
        }
    }
    return included;
}


/** Compute minimum spanning tree by [Kruskal's algorithm][1].

  We prefer Kruskal's algorithm to Prim's because here, mostly the
  number of edges grow linearly with the number of nodes, so sorting
  the edges is comparably fast.

  [1]: https://en.wikipedia.org/wiki/Kruskal%27s_algorithm
*/
std::vector<bool>
minimum_spanning_tree(const size_t n,
                      const std::vector<int> &source,
                      const std::vector<int> &target,
                      const std::vector<double> &weight)
{
    const size_t m = source.size();
    if (m != target.size())
        throw __FILE__ "source.size() != target.size()";
    if (m != weight.size())
        throw __FILE__ "source.size() != weight.size()";
    std::vector<int> edges;
    edges.resize(m);
    for (int i = 0; i < int(m); i++)
        edges[i] = i;
    std::sort(edges.begin(), edges.end(), [&](int i, int j)
              { return weight[i] < weight[j]; });
    return _spanning_tree(n, m, source, target, edges);
}


std::vector<bool>
random_spanning_tree(const size_t n,
                     const std::vector<int> &source,
                     const std::vector<int> &target,
                     const int seed = 2018)
{
    const size_t m = source.size();
    if (m != target.size())
        throw __FILE__ "source.size() != target.size()";

    std::vector<int> edges;
    edges.resize(m);
    for (int i = 0; i < int(m); i++)
        edges[i] = i;

    std::mt19937_64 generator (seed);
    std::shuffle(edges.begin(), edges.end(), generator);
    return _spanning_tree(n, m, source, target, edges);
}
