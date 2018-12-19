#pragma once
#include <set>
#include <iostream>

#include "../utils/viostream.hpp"


template<typename int_=int>
struct IndexIter
{
    const int_ *start;
    const int_ *stop;

    IndexIter(const int_ *start, const int_ *stop) :
        start(start), stop(stop) { }

    const int_* begin() const { return start; }

    const int_* end() const { return stop; }

    size_t size() const { return stop - start; }

    operator std::set<int_>() const {
        return std::set<int_>(this->begin(), this->end());
    }

    bool operator==(const std::set<int_> &o) const {
        return std::set<int_>(*this) == o;
    }
};


template<typename int_ = int>
inline std::ostream&
operator<<(std::ostream &o, const IndexIter<int_> &it)
{
    return print_it(o, it.begin(), it.end());
}


template<typename int_ = int>
struct AdjacencyIndex
{
    std::vector<int_> value;
    std::vector<int_> index;

    IndexIter<int_> operator[](int i) const {
        if (i < 0 || i >= int(value.size()))
            return {nullptr, nullptr};
        const int_
            *start = value.data() + index[i+0],
            *stop  = value.data() + index[i+1];
        return {start, stop};
    }

    size_t size() const { return index.size() -1; }
};
