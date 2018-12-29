/*
  This is NOT part of graphidx because we need the stack to perform a DFS.
*/
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
cluster<int, BiAdjacent>(
    const size_t n,
    const double *x,
    const BiAdjacent &neighidx,
    const double eps,
    const int seed);
