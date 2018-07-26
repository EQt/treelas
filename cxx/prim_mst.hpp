#pragma once
#include <random>

#include "stack.hpp"


std::vector<int>
random_spanning_tree(const BiAdjacent &index,
                     const int seed = 2018)
{
    const auto n = index.num_nodes();

    std::vector<int> parent (n, -1);
    std::mt19937_64 rng (seed);
    stack<int> stack (n);

    int root = int(rng() % n);
    parent[root] = root;
    stack.push_back(root);
    while (!stack.empty()) {
        const int v = stack.pop_back();
        for (const int u : index[v]) {
            if (parent[u] < 0) {
                parent[u] = v;
                stack.push_back(u);
                stack.swap(rng() % stack.size(), stack.size()-1);
            }
        }
    }
    return parent;
}
