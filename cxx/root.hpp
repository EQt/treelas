#pragma once
#include <vector>


template <typename int_ = int>
inline int_
find_root(const size_t n, const int_ *parent)
{
    for (int_ i = 0; i < int_(n); i++) {
        if (parent[i] == i)
            return int_(i);
    }
    return int_(-1);
}


/** Find the root node of a tree given its parent vector */
template <typename int_ = int>
inline int_
find_root(const std::vector<int_> &parent)
{
    return find_root(parent.size(), parent.data());
}
