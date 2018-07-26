#pragma once
#include <vector>
#include <random>


std::vector<int>
random_spanning_tree(const BiAdjacent &index,
                     const int seed = 2018)
{
    const auto n = index.num_nodes();

    std::vector<int> parent (n, -1);
    std::vector<int> stack_ (n, -1);

    int *stack = stack_.data();
    std::mt19937_64 rng (seed);


    int root = int(rng() % n);
    int len = 0;
    stack[len++] = root;
    while (len > 0) {
        const int v = stack[--len];
        for (const int u : index[v]) {
            if (parent[u] < 0) {
                parent[u] = v;
                stack[len++] = u;
                const int i = int(rng() % len);
                std::swap(stack[i], stack[len-1]);
            }
        }
    }
    return parent;
}
