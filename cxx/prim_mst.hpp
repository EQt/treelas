#pragma once
#include <vector>
#include <random>


std::vector<int>
random_spanning_tree(const BiAdjacent &index,
                     const int seed = 2018)
{
    const auto n = index.num_nodes();

    std::vector<int> parent (n, -1);
    std::vector<int> queue_ (n, -1);

    int *queue = queue_.data();
    std::mt19937_64 rng (seed);


    int root = int(rng() % n);
    int len = 0;
    queue[len++] = root;
    while (len > 0) {
        const int v = queue[--len];
        for (const int u : index[v]) {
            if (parent[u] < 0) {
                parent[u] = v;
                queue[len++] = u;
                const int i = int(rng() % len);
                std::swap(queue[i], queue[len-1]);
            }
        }
    }
    return parent;
}
