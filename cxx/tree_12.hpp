/**
   Approximate the tree solution by iterative refinement.
   Push formulation (each node updates its parent).
 */
#pragma once
#include <vector>
#include <array>

#include <graphidx/utils/perm.hpp>   // for iperm
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/viostream.hpp>
#include <graphidx/tree/bfs.hpp>
#include <graphidx/tree/postorder.hpp>
#include <graphidx/tree/root.hpp>

#include <graphidx/bits/minmax.hpp>
#include "tree.hpp"


namespace approx {


/** All information of a node together */
template<typename float_ = float, typename int_ = int>
struct Node
{
    float_ y;
    float_ x;
    float_ deriv;

    static constexpr int_ one =             //  b"100...0"
        (int_(1)<<(8*sizeof(int_)-1));

    inline bool same() const { return (_parent & one) != 0; }

    inline void set_same(bool v) {
        if (v) _parent |= one;
        else   _parent &= ~one;
    }

    inline int_ parent() const { return _parent & (~one); }

    inline void set_parent(const int_ p, const bool s = true) {
        if (p & one) throw std::runtime_error("too big");
        _parent = s ? (p | one) : p;
    }

private:
    int_   _parent;
};


template<typename float_ = float, typename int_ = int>
using Nodes = std::vector<Node<float_, int_>>;


/** Perform an iteration */
template<typename float_ = float, typename int_ = int>
size_t
tree12_iter(Nodes<float_, int_> &nodes,
            const int_ *preorder,
            const float_ delta,
            const float_ lam);



template<typename float_, typename int_>
void
tree_12(const size_t n,
        const float_ *y,
        const float_ lam,
        const int_ *parent,
        const int_ *order,
        const int_ *iorder,
        const int_ *ipostord,
        double *x,
        const size_t max_iter = 20);


/*
extern template
void
tree_12(const size_t n,
        const double *y,
        const double lam,
        const int *parent,
        const int *order,
        const int *iorder,
        const int *ipostord,
        double *x,
        const size_t max_iter);
*/


template<typename float_>
void
tree_12(const size_t n,
        const float_ *y,
        const float_ lam,
        const int *parent,
        double *x,
        const size_t max_iter = 20,
        int root = -1);

}   // namespace approx::


#include "tree_12.cpp"
