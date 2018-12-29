#pragma once
#include "index.hpp"
#include "groupby.hpp"

template <typename int_>
struct PartitionIter;           // forward declaration


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

    PartitionIter<int_> begin() const {
        return {this, 0};
    }

    PartitionIter<int_> end() const {
        return {this, this->size()};
    }
};


template<typename int_ = int>
struct PartitionIter
{
    const PartitionIndex<int_> *p;
    size_t i;

    PartitionIter(const PartitionIndex<int_> *p, size_t j) :
        p(p), i(j) { }

    IndexIter<int_> operator*() const {
        const int_
            *start = p->value.data() + p->index[i+0],
            *stop  = p->value.data() + p->index[i+1];

        return {start, stop};
    }

    PartitionIter<int_>& operator++() {
        this->i++;
        return *this;
    }

    bool operator==(const PartitionIter<int_> &other) const {
        return other.i == this->i;
    }
};
