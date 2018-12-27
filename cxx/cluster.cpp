#include <random>                   // std::default_random_engine
#include <algorithm>                // std::shuffle

#include "cluster.hpp"
#include "stack.hpp"


template <typename int_, typename Idx>
PartitionIndex<int_>
cluster(const size_t n,
        const double *x,
        const BiAdjacent &neighidx,
        const double eps,
        const int seed)
{
    stack<int_> s {n};
    std::vector<int_> parts (n, -1);

    // One could avoid the allocation of n*sizeof(int) by computing a
    // random permutation dynamically.  However, then we would need to
    // compute the rank operation efficiently.
    std::vector<int_> randperm;
    randperm.resize(n);
    for (int_ i = 0; i < int_(n); i++)
        randperm[i] = i;
    std::shuffle(randperm.begin(), randperm.end(),
                 std::default_random_engine(seed));

    int_ nextp = 0;
    for (const int_ i : randperm) {
        if (parts[i] < 0) {         // unexplored?
            s.push_back(i);         // DFS starting at i
            while (!s.empty()) {
                const auto v = s.pop_back();
                parts[v] = nextp;
                for (const auto u : neighidx[v]) {
                    if (std::abs(x[v] - x[u]) <= eps && parts[u] < 0)
                        s.push_back(u);
                }
            }                       // connected component is finished
            nextp++;
        }
    }
    // Can't we reuse the stack's memory for computing the PartitionIndex?
    return PartitionIndex<int_>(parts);
}


template
PartitionIndex<int>
cluster<int, BiAdjacent>(const size_t n,
                         const double *x,
                         const BiAdjacent &neighidx,
                         const double eps,
                         const int seed);
