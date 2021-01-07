#pragma once

#include <graphidx/bits/weights.hpp>
#include <graphidx/idx/biadjacent.hpp>


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

    GapMem(float_t *x, const float_t *y, size_t n, size_t m, int root = 0);

    void init();

    void find_tree();
};


template <typename float_t>
GapMem<float_t>::GapMem(float_t *x, const float_t *y, size_t n, size_t m, int root)
    : n(n), m(m), x(x), y(y), root(root)
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
void
GapMem<float_t>::find_tree()
{
}


template <typename float_t = double, typename L, typename M>
double
gaplas(
    GapMem<float_t> &mem,
    const L &lam,
    const size_t max_iter = 10,
    const M &mu = Ones<float_t>())
{
    mem.init();
    for (size_t it = 0; it < max_iter; it++) {
        mem.find_tree();
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
    return gaplas(mem, lam, max_iter, mu);
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
