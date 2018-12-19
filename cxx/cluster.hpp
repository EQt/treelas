#pragma once

#include "idx/biadjacent.hpp"
#include "idx/partition.hpp"


template <typename int_ = int>
PartitionIndex<int_>
cluster(const size_t n,
        const double *x,
        const BiAdjacent &neighidx,
        const double eps,
        const int seed);
