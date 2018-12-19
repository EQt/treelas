#pragma once
#include "index.hpp"
#include "groupby.hpp"


template <typename int_ = int>
class PartitionIndex : public AdjacencyIndex<int_>
{
public:
    PartitionIndex(const std::vector<int_> &ungrouped) :
        PartitionIndex(ungrouped.size(), ungrouped.data()) { }

    PartitionIndex(const size_t n, const int_ *ungrouped) {
        reset(n, ungrouped);
    }

    void reset(const size_t n, const int_ *ungrouped) {
        groupby(this->value,
                this->index,
                n,
                ungrouped,
                int_(-1));
    }
};
