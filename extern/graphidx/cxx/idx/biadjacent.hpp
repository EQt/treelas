#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>        // for std::max_element

#include "index.hpp"


/**
   Bidirected adjacency vectors
 **/
// template <typename int_ = int>
struct BiAdjacent : public AdjacencyIndex<int>
{
    BiAdjacent(const int m, const int *head, const int *tail, int n = -1) {
        if (n <= 0) {                   // number of nodes
            n = *std::max_element(head, head + m);
            n = std::max(n, *std::max_element(tail, tail + m));
            n++;
        }
        // auto value = this->value;
        // auto index = this->index;
        value.resize(2*m);
        index.assign(n+1, 0);
        for (int i = 0; i < m; i++) {   // compute degree
            index[head[i]+1]++;
            index[tail[i]+1]++;
        }
        {                               // shift, accumulate
            int acc = 0,
                deg_i = 0;
            for (int i = 0; i < n; i++) {
                index[i] = acc;
                acc += deg_i;
                deg_i = index[i+1];
            }
            index[n] = acc;
        }
        for (int i = 0; i < m; i++) {
            const auto
                u = head[i],
                v = tail[i];
            value[index[u+1]++] = v;
            value[index[v+1]++] = u;
        }
        if (index[n] != 2*m) {
            std::cerr << index << std::endl;
            throw std::runtime_error(std::string("\n" __FILE__) + ":" +
                                     std::to_string(__LINE__) +
                                     ": Severe Bug:\n  index[n] = " +
                                     std::to_string(index[n]) + " != " +
                                     std::to_string(2*m) + " = 2*m   ");
        }
    }

    BiAdjacent(const std::vector<int> &head,
               const std::vector<int> &tail,
               int n = -1) : BiAdjacent(int(head.size()),
                                        head.data(), tail.data(), n) {
        if (head.size() != tail.size()) {
            throw std::invalid_argument(std::string("len(head) = ") +
                                        std::to_string(head.size()) + " != " +
                                        std::to_string(tail.size()) +
                                        " = len(tail)");
        }
    }

    size_t num_edges() const { return this->value.size() / 2; }
    size_t num_nodes() const { return this->index.size() - 1; }
};


// Local Variables:
// compile-command: "cd ../build && make tests && ./tests --gtest_filter='biadj*'"
// End:
