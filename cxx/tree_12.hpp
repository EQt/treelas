/**
   Approximate the tree solution by iterative refinement.
   Push formulation (each node updates its parent).
 */
#include <vector>
#include <array>

#include "utils/perm.hpp"
#include "utils/timer.hpp"
#include "utils/viostream.hpp"
#include "utils/minmax.hpp"

#include "bfs.hpp"
#include "postorder.hpp"
#include "tree.hpp"
#include "root.hpp"


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

    inline bool same() const { return bool(_parent & one); }

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
compute_dfs_orders(const size_t n,
                   const int_ *parent,
                   int_ *order,
                   int_ *iorder,
                   int_ *ipostord)
{

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
        const size_t max_iter = 20)
{    
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
            //     print(xv, 3, stdout);
            // }
            Timer::log("%2ld ...", it+1);
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
