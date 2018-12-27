#pragma once

#include "idx/biadjacent.hpp"
#include "idx/partition.hpp"


template <typename int_ = int, typename Idx = BiAdjacent>
PartitionIndex<int_>
cluster(const size_t n,
        const double *x,
        const Idx &neighidx,
        const double eps,
        const int seed);


extern template
PartitionIndex<int>
cluster(const size_t n,
        const double *x,
        const BiAdjacent &neighidx,
        const double eps,
        const int seed);
