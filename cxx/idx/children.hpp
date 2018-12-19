#pragma once
#include <vector>
#include <stdexcept>

#include "index.hpp"

template <typename int_ = int>
void
groupby(std::vector<int_> &value,
        std::vector<int_> &index,
        const size_t n,
        const int_ *parent,
        const int_ root = -1)
{
    value.resize(n);
    index.assign(n+1, 0);       // compute number of children
    for (int i = 0; i < int(n); i++)
        index[parent[i]]++;

    if (root >= 0)
        index[root]--;              // root isn't child of itself
    {
        int acc = 0,
            deg_i = 0,
            deg_ii = index[0];
        for (int i = 0; i < int(n); i++) {
            index[i] = acc;
            acc += deg_i;
            deg_i = deg_ii;
            deg_ii = index[i+1];
        }
        index[n] = acc;
    }
    for (int v = 0; v < int(n); v++) {
        const auto p = parent[v];
        if (v == p) continue;   // skip root
        value[index[p+1]++] = v;
    }
    if (root >= 0) {
        if (index[n] != int(n-1))
            throw std::runtime_error(std::string("\n" __FILE__) + ":" +
                                     std::to_string(__LINE__) +
                                     ": Severe Bug:\n  index[n] = " +
                                     std::to_string(index[n]) + " != " +
                                     std::to_string(n-1) + " = n-1   " +
                                     "; root = " + std::to_string(root));
        value[n-1] = root;          // value should be a permutation:
                                    //   i.e. root at the end
    } else {
        if (index[n] != int(n))
            throw std::runtime_error(std::string("\n" __FILE__) + ":" +
                                     std::to_string(__LINE__) +
                                     ": Severe Bug:\n  index[n] = " +
                                     std::to_string(index[n]) + " != " +
                                     std::to_string(n-1) + " = n-1   ");
    }
}



/**
   Children vectors for all nodes
 **/
struct ChildrenIndex : public AdjacencyIndex<int>
{
    ChildrenIndex() {}


    ChildrenIndex(const size_t n) {
        index.reserve(n+1);
        value.reserve(n);
    }


    ChildrenIndex(const size_t n, const int *parent, const int root = 0) {
        reset(n, parent, root);
    }


    ChildrenIndex(const std::vector<int> &parent, const int root = 0) :
        ChildrenIndex(parent.size(), parent.data(), root) {}

    
    void reset(const size_t n, const int *parent, const int root = 0) {
        if (root < 0)
            throw std::invalid_argument(std::string("\n" __FILE__) + ":" +
                                        std::to_string(__LINE__) +
                                        ":\n root = " + std::to_string(root));
        if (parent[root] != root)
            throw std::invalid_argument(std::string("\n" __FILE__) + ":" +
                                        std::to_string(__LINE__) +
                                        ":\n  root = " + std::to_string(root) +
                                        " != " + std::to_string(parent[root]) +
                                        " = parent[root];  n = " +
                                        std::to_string(n));
        groupby(value, index, n, parent, root);
    }
};
