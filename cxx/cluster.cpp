#include "cluster.hpp"


template
PartitionIndex<int>
cluster<int, BiAdjacent>(const size_t n,
                         const double *x,
                         const BiAdjacent &neighidx,
                         const double eps,
                         const int seed);
