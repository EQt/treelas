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

    /** Represantant of a partition */
    struct Rep
    {
        int_ i;

        bool operator!=(const Rep &other) const { return i != other.i; }
        bool operator==(const Rep &other) const { return i == other.i; }
        bool operator==(const int_ &other) const { return i == other; }
    };

public:
    /** Initialize as n singleton sets. */
    UnionFind(const size_t n);

    /** Find representant for x. */
    UnionFind<int_>::Rep find(int_ x);

    /** Unit the sets with representants fx und fy. */
    void unite(UnionFind<int_>::Rep fx, UnionFind<int_>::Rep fy);

    /** Reset such that every element is separated, again */
    void reset();

    /** Number of elements */
    size_t size() const { return p.size(); }
};


template <typename int_>
UnionFind<int_>::UnionFind(const size_t n)
    : p(n), rank(n)
{
    reset();
}


template <typename int_>
void
UnionFind<int_>::reset()
{
    const auto n = int_(size());
    for (int_ i = 0; i < n; i++) {
        p[i] = i;
        rank[i] = 0;
    }
}


template <typename int_>
inline void
UnionFind<int_>::unite(UnionFind<int_>::Rep fx, UnionFind<int_>::Rep fy)
{
    if (rank[fx.i] > rank[fy.i])
        p[fy.i] = fx.i;
    else {
        p[fx.i] = fy.i;
        if (rank[fx.i] == rank[fy.i])
            rank[fy.i]++;
    }
}


template <typename int_>
typename UnionFind<int_>::Rep
UnionFind<int_>::find(int_ x)
{
    if (p[x] != x)
        p[x] = find(p[x]).i;
    return Rep {p[x]};
}

