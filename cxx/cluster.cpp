#include <random>                   // std::default_random_engine
#include <algorithm>                // std::shuffle

#include "idx/biadjacent.hpp"
#include "idx/partition.hpp"
#include "stack.hpp"


template <typename int_ = int>
PartitionIndex<int_>
cluster(const size_t n,
        const double *x,
        const BiAdjacent &neighidx,
        const double eps,
        const int seed)
{
    stack<int_> s {n};
    std::vector<int_> parts (n, -1);
    std::vector<int_> randperm;
    randperm.resize(n);
    for (size_t i = 0; i < n; i++)
        randperm[i] = i;

    std::shuffle(randperm.begin(), randperm.end(),
                 std::default_random_engine(seed));

    int_ nextp = 0;
    for (const auto i : randperm) {
        if (parts[i] < 0) {         // unexplored?
            s.push_back(i);         // DFS from i
            while (!s.empty()) {
                const auto v = s.pop_back();
                parts[v] = nextp;
                for (const auto u : neighidx[v]) {
                    if (std::abs(x[v] - x[u]) < eps && parts[u] < 0)
                        s.push_back(u);
                }
            }
            nextp++;
        }
    }
    return PartionIndex(parts);
}
