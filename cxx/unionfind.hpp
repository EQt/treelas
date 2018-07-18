#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <vector>

/**
 * Union-Find data structure over ints from range 0..n with path compression.
 * See Cormen, Leiserson, Rivest und Stein: "Introduction to Algorithms",
 * Section 21.3, in particular pseudocode on page 571.
 */
class UnionFind
{
    std::vector<int> p;
    std::vector<int> rank;

public:
    /** Initialize as n singleton sets. */
    UnionFind(int n);

    /** Find representant for x. */
    int find(int x);

    /** Unit the sets with representants fx und fy. */
    void unite(int fx, int fy);
};


UnionFind::UnionFind(int n)
    : p(n), rank(n)
{
    for (int i = 0; i < n; i++) {
        p[i] = i;
        rank[i] = 0;
    }
}


void
UnionFind::unite(int fx, int fy)
{
    if (rank[fx] > rank[fy])
        p[fy] = fx;
    else {
        p[fx] = fy;
        if (rank[fx] == rank[fy])
            rank[fy]++;
    }
}


int
UnionFind::find(int x)
{
    if (p[x] != x)
        p[x] = find(p[x]);
    return p[x];
}


#endif /* UNIONFIND_HPP */
