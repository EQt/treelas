#pragma once
#include <vector>
#include <stdexcept>

#include "groupby.hpp"
#include "index.hpp"
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
