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
compute_orders(TreeLasso<float_, int_> &tree,
               std::vector<int_> &ipostord,
               std::vector<int_> &iorder,
               const bool use_dfs,
               const size_t PRINT_MAX = 0)
{
    const auto n = tree.parent.size();
    {   Timer _ ("Computing BFS");
        if (use_dfs) {
            tree.preorder = std::move(tree.dfs);
        } else {
            tree.preorder = (tree.bfs.size() == n) ?
                tree.bfs : compute_bfs(tree.parent, tree.root);
        }
        // for testing:
        // preorder = {0, 2, 3, 4, 1, 5, 6, 7};
        tree.postorder = reverse(tree.preorder);
        ipostord = iperm(tree.postorder);
        iorder = concat(ipostord, tree.preorder);
    }
    if (n <= PRINT_MAX) {
        printf("   parent: ");
        print(tree.parent, 0);
        printf(" preorder: ");
        print(tree.preorder, 0);
        printf("postorder: ");
        print(tree.postorder, 0);
        printf(" ipostord: ");
        print(ipostord, 0);
    }
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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template<typename float_>
void
tree_12(const size_t n,
        const float_ *y,
        const float_ lam,
        const int *parent,
        double *x,
        const size_t max_iter = 20,
        int root = -1)
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
        post_order(root, childs, stack, postord.data());
    }
    {   Timer _ ("iorder");
        iperm(n, iorder.data(), postord.data());
    }
}
#pragma GCC diagnostic pop


template<typename float_, typename int_>
std::vector<double>
tree_12(const TreeLasso<float_, int_> &tree,
        const std::vector<int_> &ipostordv,
        const std::vector<int_> &iorderv,
        const size_t max_iter = 20)
{
    const auto n = tree.parent.size();
    std::vector<double> xv (n);

    const float_ lam = tree.lam[0];
    const float_ *y = tree.y.data();
    const int_ *parent = tree.parent.data();
    const int  *order = tree.postorder.data();
    const int_ *ipostord = ipostordv.data();
    const int_ *iorder = iorderv.data();
    double *x = xv.data();

    tree_12(n, y, lam, parent, order, iorder, ipostord, x, max_iter);

    return xv;
}

}   // namespace approx::
