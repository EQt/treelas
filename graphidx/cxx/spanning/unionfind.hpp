#pragma once

#include <vector>

/**
 * Union-Find data structure over ints from range 0..n with path compression.
 * See Cormen, Leiserson, Rivest und Stein: "Introduction to Algorithms",
 * Section 21.3, in particular pseudocode on page 571.
 */
template <typename int_ = int>
class UnionFind
{
    std::vector<int_> p;
    std::vector<int_> rank;

public:
    /** Initialize as n singleton sets. */
    UnionFind(size_t n);

    /** Find representant for x. */
    int_ find(int_ x);

    /** Unit the sets with representants fx und fy. */
    void unite(int_ fx, int_ fy);
};


template <typename int_>
UnionFind<int_>::UnionFind(size_t n)
    : p(n), rank(n)
{
    for (int_ i = 0; i < int_(n); i++) {
        p[i] = i;
        rank[i] = 0;
    }
}


template <typename int_>
void
UnionFind<int_>::unite(int_ fx, int_ fy)
{
    if (rank[fx] > rank[fy])
        p[fy] = fx;
    else {
        p[fx] = fy;
        if (rank[fx] == rank[fy])
            rank[fy]++;
    }
}


template <typename int_>
int_
UnionFind<int_>::find(int_ x)
{
    if (p[x] != x)
        p[x] = find(p[x]);
    return p[x];
}

