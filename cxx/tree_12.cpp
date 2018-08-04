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
