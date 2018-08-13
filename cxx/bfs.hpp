/*
  Utility functions around breadth-first-search (BFS), and tree level
  computation.
*/
#pragma once
#ifndef STD_QUEUE
#   define STD_QUEUE 0
#endif

#include <vector>
#include <stdexcept>
#if STD_QUEUE
#  include <queue>
#else
#  include "queue.hpp"
#endif

#include "children.hpp"


inline std::vector<int>
compute_bfs(const std::vector<int> &parent, const int root)
{
    const auto n = parent.size();
    ChildrenIndex children (parent, root);
    std::vector<int> bfs (n, -1);
    int b = 0;
#if STD_QUEUE
    std::queue<int> q;
#else
    queue<int> q (n);
#endif

    q.push(root);
    while (!q.empty()) {
        auto u = q.front(); q.pop();
        bfs[u] = b++;
        for (auto v : children[u]) {
            if (v != root)
                q.push(v);
        }
    }
    return bfs;
}


inline std::vector<int>
compute_levels(const std::vector<int> &bfs,
               const std::vector<int> &parent)
{
    const auto n = bfs.size();
    std::vector<int> levels, depth(n, 0);
    levels.reserve(n);
    int d = 0;
    levels.push_back(0);   // 0th level: consists only of root = bfs[0]
    for (unsigned i = 1; i < n; i++) {
        const auto b = bfs[i];
        const auto db = 1 + depth[parent[b]];
        depth[b] = db;
        if (db > d) {
            d++;
            levels.push_back(i);
        }
    }
    levels.push_back(int(n));
    return levels;
}
