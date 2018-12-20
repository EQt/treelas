#pragma once
#include <vector>
#include <stdexcept>

#include "utils/viostream.hpp"

/** See `pruefer2edges` for details.

    Parameters:
      parent    Output, len(parent) == n

    Returns: a root node
 */
template<typename int_ = int>
inline int_
prufer2parent(const int_ n,
              const int_ *prufer,
                    int_ *parent)
{
    for (int_ i = 0; i < n; i++)
        parent[i] = -1;
    std::vector<int_> degree (n, 1);
    for (int_ i = 0; i < n-2; i++) {
        const auto pi = prufer[i];
        if (pi < 0 || pi >= n)
            throw std::invalid_argument(std::string("prufer[") +
                                        std::to_string(i) + "] = " +
                                        std::to_string(pi) + "; n = " +
                                        std::to_string(n));
        degree[pi]++;
    } // degree[v] is now the degree of node v

    int_ index = 0, u = 0;
    auto find_min = [&](int_ start) { // index = minimal k with degree[k] == 1
        index = start;
        while (index < n && degree[index] > 1) 
            index++;
        if (degree[index] != 1)
            throw std::runtime_error(
                std::string("This should not happen: degree[") +
                std::to_string(index) + "] = " +
                std::to_string(degree[index]));
        u = index;
    };

    find_min(0);
    for (int_ i = 0; i < n-2; i++) {
        const auto v = prufer[i];
        parent[u] = v;
        degree[v]--;
        if (v < index && degree[v] == 1)
            u = v;
        else {
            find_min(++index);
        }
    }

    while (u < n && parent[u] >= 0)
        u++;
    if (u >= n)
        throw std::runtime_error(std::string("Strange: u = ") +
                                 std::to_string(u) + "; n = " +
                                 std::to_string(n));
    index = u+1;
    while (index < n && parent[index] >= 0)
        index++;
    if (index >= n)
        throw std::runtime_error(std::string("Strange: index = ") +
                                 std::to_string(index) + "; n = " +
                                 std::to_string(n));

    parent[u] = index;
    parent[index] = index;
    return index;
}


/** Calculate the tree edges from a Prüfer sequence.

  Parameters:
    n       number of nodes in the tree
    prufer  Prüfer sequence (len(prufer) == n - 2)
    head    Output edges' head (len(head) >= n-1)
    tail    Output edges' tail (len(tail) >= n  )

  Runtime: O(n).

  References
  ----------
    Wang, Xiaodong, Lei Wang, and Yingjie Wu.
    "An optimal algorithm for Prufer codes."
    *Journal of Software Engineering and Applications* 2.02 (2009): 111.
    <https://doi.org/10.4236/jsea.2009.22016>
*/
template<typename int_ = int>
inline void
prufer2edges(const int n,
             const int_ *prufer,
             int_ *head,
             int_ *tail)
{
    prufer2parent(n, prufer, tail);
    for (int i = 0, j = 0; i < n; i++) {
        if (tail[i] != i) { // skip root node
            head[j] = j;
            tail[j] = tail[i];
            j++;
        }
    }
}
