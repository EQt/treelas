#pragma once

#include <graphidx/bits/weights.hpp>
#include <graphidx/idx/biadjacent.hpp>
#include <graphidx/spanning/prim_mst.hpp>

#include "edges.hpp"
#include "tree_dp.hpp"


using GraphIndex = BiAdjacentIndex<int>;


template <typename float_t = double>
struct GapMem
{
    const size_t n, m;
    float_t *x;
    const float_t *y;
    const int root;
    std::vector<float_t> y_tree, alpha, alpha_tree, gamma;
    std::vector<int> parent;
    TreeDPStatus mem_tree;

    GapMem(float_t *x, const float_t *y, size_t n, size_t m, int root = 0);

    void init();

    template <typename L, typename int_t = int>
    void find_tree(const GraphIndex &graph, const L &lam);
};


template <typename float_t>
GapMem<float_t>::GapMem(float_t *x, const float_t *y, size_t n, size_t m, int root)
    : n(n), m(m), x(x), y(y), root(root), mem_tree(n)
{
    y_tree.resize(n);
    alpha.resize(n);
    alpha_tree.resize(m);
    gamma.resize(m);
    parent.resize(n);
}


template <typename float_t>
void
GapMem<float_t>::init()
{
    for (size_t i = 0; i < n; i++)
        x[i] = y[i];
    for (size_t e = 0; e < m; e++)
        alpha[e] = 0.0;
}


template <typename float_t>
template <typename L, typename int_t>
void
GapMem<float_t>::find_tree(const GraphIndex &graph, const L &lam)
{
    // update gamma
    size_t e = 0;
    const auto c = -1;
    edges<int_t>(graph, [&](int_t u, int_t v) {
        auto diff = x[u] - x[v];
        gamma[e] = c * (lam[e] * std::abs(diff) + alpha[e] * diff);
	e++;
    });
    // minimum spanning tree

    // update parent
}


template <typename float_t = double, typename L, typename M>
double
gaplas(
    GapMem<float_t> &mem,
    const GraphIndex &graph,
    const L &lam,
    const size_t max_iter = 10,
    const M &mu = Ones<float_t>())
{
    constexpr bool merge_sort = false, lazy_sort = false;

    mem.init();
    for (size_t it = 0; it < max_iter; it++) {
        mem.find_tree(graph, lam);
        const auto &tree_lam = lam; // FIXME
        tree_dp<merge_sort, lazy_sort>(
            mem.n,
            mem.x,
            mem.y_tree.data(),
            mem.parent.data(),
            tree_lam,
            mu,
            mem.root,
            mem.mem_tree);
    }
    return -1;
}


template <typename float_t = double, typename Wlam, typename Wmu>
double
gaplas(
    float_t *x,
    const float_t *y,
    const GraphIndex &idx,
    const Wlam &lam,
    const Wmu &mu,
    const size_t max_iter)
{
    int root = 0;
    GapMem<float_t> mem(x, y, idx.num_nodes(), idx.num_edges(), root);
    return gaplas(mem, idx, lam, max_iter, mu);
}


template <typename float_t = double, typename Wlam>
double
gaplas(
    float_t *x,
    const float_t *y,
    const GraphIndex &idx,
    const Wlam &lam,
    const size_t max_iter)
{
    return gaplas(x, y, idx, lam, Ones<float_t>(), max_iter);
}
