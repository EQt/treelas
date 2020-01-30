/**
   Approximate the tree solution by iterative refinement.
   Push formulation (each node updates its parent).
 */
#pragma once
#include <vector>
#include <array>

#include <graphidx/tree/bfs.hpp>
#include <graphidx/tree/postorder.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/perm.hpp>   // for invperm
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/viostream.hpp>

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


#include <cassert>
#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/minmax.hpp>


namespace approx {


template<typename float_, typename int_>
size_t
tree12_iter(Nodes<float_, int_> &nodes,
            const int_ *preorder,
            const float_ delta,
            const float_ lam)
{
    const auto n = nodes.size();
    size_t changed = 0;

    for (auto &v : nodes)
        v.deriv = v.x - v.y;

    {   Timer _ ("forward");
        for (const auto &v : nodes) {
            auto &p = nodes[v.parent()];
            if (v.same())
                p.deriv += clamp(v.deriv, -lam, +lam);
        }
    }
 
    auto &r = nodes[preorder[0]];
    r.x += r.deriv < 0 ? +delta : -delta;      // optimize root node

    {   Timer _ ("backward");
        for (size_t i = 1; i < n; i++) {       // backtracing
            auto &v = nodes[preorder[i]];
            auto &p = nodes[v.parent()];
            if (v.same()) {                    // same region, so far?
                if (v.deriv > lam) {
                    v.x -= delta;
                } else if (v.deriv < -lam) {
                    v.x += delta;
                } else {
                    v.x = p.x;
                    continue;
                }

                if (v.x != p.x) {               // update whether still same
                    v.set_same(false);
                    changed++;
                    if (v.x < p.x) {
                        v.y += lam;
                        p.y -= lam;
                    } else {
                        v.y -= lam;
                        p.y += lam;
                    }
                }
            } else
                v.x += v.deriv < 0 ? +delta : -delta;
        }
    }
    return changed;
}


template<typename float_>
void
tree_12(const size_t n,
        const float_ *y,
        const float_ lam,
        const int *parent,
        double *x,
        const size_t max_iter,
        int root)
{

    if (root < 0) {
        Timer _ ("find root");
        root = find_root(n, parent);
    }

    std::vector<int> postord, iorder, ipostord;
    stack<int> stack;

    Timer tim ("children index");
    ChildrenIndex childs (n, parent, root);
    tim.stop();

    {   Timer _ ("allocate orders");
        postord.reserve(n);
        iorder.reserve(n);
        ipostord.reserve(n);
        stack.reserve(2*n);
    }
    {   Timer _ ("postorder");
        post_order(postord.data(), childs, stack);
    }
    {   Timer _ ("iorder");
        invperm(n, iorder.data(), postord.data());
    }

    for (size_t it = 0; it < max_iter; it++) {
        ;
    }

    for (size_t i = 0; i < n; i++)
        x[i] = y[i] + 0 * lam;
}


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
        const size_t max_iter)
{
    assert(order);
    float_ y_min, y_max;
    {   Timer _ ("Analyze y");
        find_minmax(y, n, y_min, y_max);
    }
    float_
        delta = float_(0.5 * (y_max - y_min)),
        y_mid = float_(0.5 * (y_max + y_min));

    std::vector<Node<float_, int_>> nodes;
    {   Timer _ ("Init nodes");
        nodes.resize(n);
        for (size_t i = 0; i < n; i++) {
            const int ii = order[i];
            nodes[i].y = y[ii];
            nodes[i].x = y_mid;
            nodes[i].set_parent(ipostord[parent[ii]], true);
        }
    }

    {   Timer _ ("Iterations:\n");
        for (size_t it = 0; it < max_iter; it++) {
            delta *= float_(0.5);
            // if (n <= 20) {
            //     printf("delta=%.4f;   x: ", delta);
            //     for (size_t i = 0; i < n; i++)
            //         x[order[i]] = nodes[i].x;
            //     print(x, 3, stdout);
            // }
            Timer::log("%2ld ...\n", it+1);
            const auto changed = tree12_iter(nodes, iorder, delta, lam);
            if (changed)
                Timer::log("  %d", changed);
            Timer::log("\n");
        }
    }

    {   Timer _ ("Extract x");
        for (size_t i = 0; i < n; i++)
            x[order[i]] = nodes[i].x;
    }
}


} // namespace approx::
