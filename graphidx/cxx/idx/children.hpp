#pragma once
#include <vector>
#include <stdexcept>

#include "groupby.hpp"
#include "index.hpp"
#include "../tree/root.hpp"


/**
   Access the children of each node of the tree.

   By convention, the root node is stored at ChildrenIndex::value[0].
*/
struct ChildrenIndex : public AdjacencyIndex<int>
{
    ChildrenIndex() {}


    ChildrenIndex(const size_t n) {
        index.reserve(n+1);
        value.reserve(n);
    }


    ChildrenIndex(const size_t n, const int *parent, const int root = -1) {
        reset(n, parent, root);
    }


    ChildrenIndex(const std::vector<int> &parent, const int root = -1) :
        ChildrenIndex(parent.size(), parent.data(), root) {}

    
    void reset(const size_t n, const int *parent, int root = -1) {
        if (root < 0)
            root = find_root(n, parent);

        if (root < 0 || parent[root] != root)
            throw std::invalid_argument(std::string("\n" __FILE__) + ":" +
                                        std::to_string(__LINE__) +
                                        ":\n  root = " + std::to_string(root) +
                                        " != " + std::to_string(parent[root]) +
                                        " = parent[root];  n = " +
                                        std::to_string(n));
        groupby(value, index, n, parent, root);
    }


    int root_node() const { return this->value[0]; }
};
