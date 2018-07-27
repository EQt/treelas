#pragma once
#include <random>

#include "stack.hpp"


void
random_spanning_tree(const BiAdjacent &index,
                     const int seed,
                     int *parent)
{
    const auto n = index.num_nodes();
    std::mt19937_64 rng (seed);
    stack<int> stack (n);

    for (size_t i = 0; i < n; i++)
        parent[i] = -1;

    int root = int(rng() % n);
    parent[root] = root;
    stack.push_back(root);
    while (!stack.empty()) {
        const int v = stack.pop_back();
        for (const int u : index[v]) {
            if (u == v)
                continue;
            if (parent[u] == -1) {
                stack.push_back(u);
                parent[u] = v;
            } else if (rng() % 3)
                parent[u] = v;
            if (stack.size() > 0)
                stack.swap(rng() % stack.size(), stack.size()-1);
        }
    }
}


std::vector<int>
random_spanning_tree(const BiAdjacent &index,
                     const int seed = 2018)
{
    std::vector<int> parent (index.num_nodes());
    random_spanning_tree(index, seed, parent.data());
    return parent;
}
