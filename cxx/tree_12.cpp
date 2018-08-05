#include "tree_12.hpp"


template<typename float_ = float, typename int_ = int>
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

    for (const auto &v : nodes) {
        auto &p = nodes[v.parent()];
        if (v.same())
            p.deriv += clip(v.deriv, -lam, +lam);
    }

    auto &r = nodes[preorder[0]];
    r.x += r.deriv < 0 ? delta : -delta;   // optimize root node

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
            }

            if (v.x != p.x) {               // update whether still same
                v.set_same(false);
                changed++;
                if (v.x < p.x) {
                    p.y -= lam;
                    v.y += lam;
                } else {
                    p.y += lam;
                    v.y -= lam;
                }
            }
        } else
            v.x += v.deriv < 0 ? +delta : -delta;
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
