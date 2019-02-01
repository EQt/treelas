#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>        // std::max_element
#include <sstream>

#include "utils/viostream.hpp"


/** Group the elements in `parent` (stored in `value`, group index `index`)

    If `root >= 0` treat it as the root of the tree given by `parent`.

    Algorithm: Counting Sort
    https://en.wikipedia.org/wiki/Counting_sort

    Linear Runtime.
*/
template <typename int_ = int>
void
groupby(std::vector<int_> &value,
        std::vector<int_> &index,
        const size_t n,
        const int_ *parent,
        const int_ root = -1)
{
    value.resize(n);
    size_t k = root >= 0 ? n :
        size_t(*std::max_element(parent, parent + n) + 1);

    index.assign(k+1, 0);           // compute histogram, i.e. number of children
    for (int i = 0; i < int(n); i++)
        index[parent[i]]++;

    if (root >= 0)
        index[root]--;              // root isn't child of itself

    {                               // accumulate prefix sums
        int_ acc = 0,
             deg_i = 0,
             deg_ii = index[0];
        for (int i = 0; i < int(k); i++) {
            index[i] = acc;
            acc += deg_i;
            deg_i = deg_ii;
            deg_ii = index[i+1];
        }
        index[k] = acc;
    }

    for (int_ v = 0; v < int_(n); v++) {  // sort the values
        const auto p = parent[v];
        if (v == root) continue;          // skip root
        value[index[p+1]++] = v;
    }

    if (root >= 0) {
        if (index[n] != int_(n-1))
            throw std::runtime_error(std::string("\n" __FILE__) + ":" +
                                     std::to_string(__LINE__) +
                                     ": Severe Bug:\n  index[n] = " +
                                     std::to_string(index[n]) + " != " +
                                     std::to_string(n-1) + " = n-1   " +
                                     "; root = " + std::to_string(root));
        value[n-1] = root;          // value should be a permutation:
                                    //   i.e. root at the end
    } else {
        if (index[k] != int_(n)) {
            std::ostringstream msg;
            msg << "\n" << __FILE__ << ":" <<__LINE__
                << ": Severe Bug:\n  index[k] = " << index[k]
                << " != " << n << " = n   ";
            if (k <= 10)
                msg << std::endl
                    << "  index = " << index << std::endl;
            throw std::runtime_error(msg.str());
        }
    }
}
