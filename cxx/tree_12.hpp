#include "utils/perm.hpp"
#include "utils/timer.hpp"
#include "utils/viostream.hpp"
#include "utils/minmax.hpp"

#include "bfs.hpp"
#include "tree.hpp"

namespace approx {


#ifndef EXTRA
#  define EXTRA 0       // number of extra bytes in Node (for padding)
#endif
template<typename float_ = float,
         typename int_ = int>
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

    inline void set_parent(int_ p) {
        if (p & one) throw std::runtime_error("too big");
        _parent = p;
    }

private:
    int_   _parent;

#if EXTRA > 0
    char   _ignore[EXTRA];
#endif
};


template<typename float_ = float, typename int_ = int>
inline void
update_x(Node<float_, int_> &v,
         Node<float_, int_> &p,
         const float_ lam,
         const float_ delta,
         const float_ c,
         int &changed)
{
    if (v.same()) {
        if (v.deriv > lam) {
            v.x -= c;
        } else if (v.deriv < -lam) {
            v.x += c;
        } else {
            v.x = p.x;
        }

        if (v.x != p.x) {               // update whether still same
            v.set_same(false);
            changed++;
            if (v.x < p.x) {
                p.y += lam;
                v.y -= lam;
            } else {
                p.y -= lam;
                v.y += lam;
            }
        }
    } else {
        v.x += v.deriv < 0 ? c : -c;
    }
}


template<typename float_ = float, typename int_ = int>
int
tree12_iter(std::vector<Node<float_, int_>> &nodes,
            const int_ *preorder,
            const float_ delta,
            const float_ lam,
            const float_ mu = float_(0.5))
{
    int changed = 0;
    const int n = nodes.size();

    float_ c = float_(0.5 * delta);
    for (auto &v : nodes) {
        v.deriv = float_(2.0) * mu * v.x + v.y;
    }

    static bool first = n < 20;
    for (const auto &v : nodes) {
        auto &p = nodes[v.parent()];
        if (first)
            Timer::log("nodes.deriv = %f\n", p.deriv);
        if (v.same()) {
            p.deriv += clip(v.deriv, -lam, +lam);
        }
    }

    if (first)
        first = false;

    // optimize root node
    auto &r = nodes[preorder[0]];
    r.x += r.deriv < 0 ? c : -c;

    // backtracing
    for (int i = 1; i < n; i++) {
        auto &v = nodes[preorder[i]];
        auto &p = nodes[v.parent()];
        update_x(v, p, lam, delta, c, changed);
    }
    return changed;
}


template<typename float_, typename int_>
void
compute_orders(TreeLasso<float_, int_> &tree,
               std::vector<int_> &ipostord,
               std::vector<int_> &iorder,
               const bool use_dfs,
               const int PRINT_MAX = 0)
{
    const int n = tree.parent.size();
    {   Timer _ ("Computing BFS");
        if (use_dfs) {
            tree.preorder = std::move(tree.dfs);
        } else {
            tree.preorder = (int(tree.bfs.size()) == n) ?
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
std::vector<double>
tree12(const TreeLasso<float_, int_> &tree,
       const std::vector<int_> &ipostordv,
       const std::vector<int_> &iorderv,
       const int max_iter = 20)
{
    using namespace disc;
    const int n = tree.parent.size();
    std::vector<double> xv (n);

    const float_ mu = float_(0.5);
    const float_ lam = tree.lam[0];
    const float_ *y = tree.y.data();
    const int_ *parent = tree.parent.data();
    const int  *order = tree.postorder.data();
    const int_ *ipostord = ipostordv.data();
    const int_ *iorder = iorderv.data();
    double *x = xv.data();

    float_ y_min, y_max;
    {   Timer _ ("Analyze y");
        find_minmax(y, n, y_min, y_max);
    }
    float_
        delta = 0.5 * (y_max - y_min),
        y_mid = 0.5 * (y_max + y_min);

    std::vector<Node<float_, int_>> nodes;
    {   Timer _ ("Init nodes");
        nodes.resize(n);
        for (int i = 0; i < n; i++) {
            const int ii = order[i];
            nodes[i].y = -2.0 * mu * y[ii];
            nodes[i].x = y_mid;
            nodes[i].set_parent(ipostord[parent[ii]]);
            nodes[i].set_same(true);
        }
    }

    {   Timer _ ("Iterations:\n");
        for (int it = 0; it < max_iter; it++) {
            Timer::log("%2d ...", it+1);
            int changed = tree12_iter(nodes, iorder, delta, lam, mu);
            if (changed)
                Timer::log("  %d", changed);
            Timer::log("\n");
            delta /= 2.0;
        }
    }

    {   Timer _ ("Extract x");
        for (int i = 0; i < n; i++) {
            x[order[i]] = nodes[i].x;
        }
    }

    return xv;
}

}   // namespace approx::
