#pragma once

#include <graphidx/bits/weights.hpp>
#include <graphidx/edges.hpp>
#include <graphidx/idx/incidence.hpp>
#include <graphidx/spanning/prim_mst.hpp>
#include <stdexcept>
#include <type_traits>

#include "tree_dp.hpp"


template <typename float_t = double, typename int_t = int>
struct GapMem
{
    static_assert(std::is_integral<int_t>::value, "need integers for indexing");

    const size_t n, m;
    float_t *x;
    const float_t *y;
    const int_t root;
    std::vector<float_t> alpha, gamma;
    std::vector<float_t> y_tree, alpha_tree;
    std::vector<int_t> parent;
    TreeDPStatus mem_tree;

    GapMem() = delete;

    GapMem(float_t *x, const float_t *y, size_t n, size_t m, int_t root = 0);

    void init();

    template <typename L>
    void gap_vec(const IncidenceIndex<int_t> &graph, const L &lam);

    template <typename Queue, typename L>
    void find_tree(const IncidenceIndex<int_t> &graph, L &tlam, const L &lam);

    void update_duals(const IncidenceIndex<int_t> &graph);

    template <typename L, typename M>
    void tree_opt(const L &tree_lam, const M &mu);

    template <typename Queue, typename L, typename M>
    void next(const IncidenceIndex<int_t> &graph, L &tlam, const L &lam, const M &mu);

    template <typename L, typename M>
    double
    primal_obj(const IncidenceIndex<int_t> &idx, const L &lam, const M &mu) const;

    double gap_obj() const;
};


template <typename float_t, typename int_t>
double
GapMem<float_t, int_t>::gap_obj() const
{
    double sum = 0;
    for (size_t e = 0; e < m; e++)
        sum += double(gamma[e]);
    return -sum;
}


template <typename float_t, typename int_t>
template <typename L, typename M>
double
GapMem<float_t, int_t>::primal_obj(
    const IncidenceIndex<int_t> &idx, const L &lam, const M &mu) const
{
    double sum = 0;
    for (size_t i = 0; i < n; i++) {
        const auto diff = double(x[i]) - double(y[i]);
        sum += mu[i] * diff * diff;
    }
    edges<int_t>(idx, [&](int_t u, int_t v, int_t e) {
        sum += u < v ? lam[e] * std::abs(double(x[u]) - double(x[v])) : 0.0;
    });
    return sum;
}


template <typename float_t, typename int_t>
template <typename Queue, typename L, typename M>
void
GapMem<float_t, int_t>::next(
    const IncidenceIndex<int_t> &graph, L &tlam, const L &lam, const M &mu)
{
    gap_vec(graph, lam);
    find_tree<Queue>(graph, tlam, lam);
    tree_opt(tlam, mu);
    update_duals(graph);
}


template <typename int_t = int, typename float_t = double>
void
tree_dual(const size_t n, float_t *alpha, const int_t *parent, const int_t *postorder)
{
    for (size_t i = 0; i < n - 1; i++) {
        const auto v = postorder[i];
        alpha[parent[v]] += alpha[v];
    }
}


template <typename int_t = int, typename float_t = double>
void
tree_dual(
    const size_t n,
    float_t *alpha,
    const float_t *x,
    const float_t *y,
    const int_t *parent,
    const int_t *postorder)
{
    for (size_t i = 0; i < n; i++)
        alpha[i] = x[i] - y[i];
    tree_dual(n, alpha, parent, postorder);
}


template <typename float_t, typename int_t>
template <typename L, typename M>
void
GapMem<float_t, int_t>::tree_opt(const L &tree_lam, const M &mu)
{
    constexpr bool merge_sort = false, lazy_sort = false;
    tree_dp<merge_sort, lazy_sort>(
        n, x, y_tree.data(), parent.data(), tree_lam, mu, root, mem_tree);
}


template <typename float_t, typename int_t>
void
GapMem<float_t, int_t>::update_duals(const IncidenceIndex<int_t> &graph)
{
    tree_dual(
        n,
        alpha_tree.data(),
        x,
        y_tree.data(),
        parent.data(),
        mem_tree.proc_order.data());
    edges<int>(graph, [&](int_t u, int_t v, int_t e) {
        if (u < v) {
            if (parent[u] == v)
                alpha[e] = alpha_tree[u];
            if (parent[v] == u)
                alpha[e] = -alpha_tree[v];
        }
    });
}


template <typename float_t, typename int_t>
GapMem<float_t, int_t>::GapMem(
    float_t *x, const float_t *y, size_t n, size_t m, int_t root)
    : n(n), m(m), x(x), y(y), root(root), mem_tree(n)
{
    alpha.resize(m);
    gamma.resize(m);
    y_tree.resize(n);
    alpha_tree.resize(n);
    parent.resize(n);
}


template <typename float_t, typename int_t>
void
GapMem<float_t, int_t>::init()
{
    for (size_t i = 0; i < n; i++)
        x[i] = y[i];
    for (size_t e = 0; e < m; e++)
        alpha[e] = 0.0;
}

template <typename float_t, typename int_t>
template <typename L>
void
GapMem<float_t, int_t>::gap_vec(const IncidenceIndex<int_t> &graph, const L &lam)
{
    // update gamma
    const auto c = -1;
    edges<int_t>(graph, [&](int_t u, int_t v, int_t e) {
        if (false) {
            if (e < 0 || e >= int_t(m))
                throw std::runtime_error(
                    std::to_string(e) + ", m = " + std::to_string(m));
            if (u < 0 || u >= int_t(n))
                throw std::runtime_error(
                    std::to_string(u) + " = u, n = " + std::to_string(n));
            if (v < 0 || v >= int_t(n))
                throw std::runtime_error(
                    std::to_string(v) + " = v, n = " + std::to_string(n));
        }
        if (u < v) {
            auto diff = x[u] - x[v];
            gamma[e] = c * (lam[e] * std::abs(diff) + alpha[e] * diff);
        }
    });
}


template <typename float_t, typename int_t>
template <typename Queue, typename L>
void
GapMem<float_t, int_t>::find_tree(
    const IncidenceIndex<int_t> &graph, L &tlam, const L &lam)
{
    // minimum spanning tree: update parent
    prim_mst_edges<Queue>(parent.data(), gamma.data(), graph, root);
    for (size_t i = 0; i < n; i++)
        y_tree[i] = y[i];
    edges<int_t>(graph, [&](int_t u, int_t v, int_t e) {
        if (u < v) {
            if (parent[u] == v)
                tlam[u] = lam[e];
            else if (parent[v] == u)
                tlam[v] = lam[e];
            else {
                y_tree[u] += alpha[e];
                y_tree[v] -= alpha[e];
            }
        }
    });
}


template <
    typename Queue,
    typename float_t = double,
    typename int_t = int,
    typename L,
    typename M>
double
gaplas(
    GapMem<float_t, int_t> &mem,
    const IncidenceIndex<int_t> &graph,
    const L &lam,
    const size_t max_iter = 10,
    const bool verbose = true,
    const M &mu = Ones<float_t>())
{
    mem.init();
    L tlam(lam);
    for (size_t it = 0; it < max_iter; it++) {
        if (verbose)
            std::cout << it << '\t' << mem.primal_obj(graph, lam, mu) << '\t'
                      << mem.gap_obj() << std::endl;
        mem.template next<Queue>(graph, tlam, lam, mu);
    }
    return -1; // TODO: return runtime
}


template <
    typename Queue,
    typename float_t = double,
    typename int_t = int,
    typename L,
    typename M>
double
gaplas(
    float_t *x,
    const float_t *y,
    const IncidenceIndex<int_t> &idx,
    const L &lam,
    const M &mu,
    const size_t max_iter,
    const bool verbose)
{
    int root = 0;
    GapMem<float_t, int_t> mem(x, y, idx.num_nodes(), idx.num_edges(), root);
    return gaplas<Queue>(mem, idx, lam, max_iter, verbose, mu);
}


template <typename Queue, typename float_t = double, typename int_t, typename L>
double
gaplas(
    float_t *x,
    const float_t *y,
    const IncidenceIndex<int_t> &idx,
    const L &lam,
    const size_t max_iter,
    const bool verbose = true)
{
    const auto mu = Ones<float_t>();
    return gaplas<Queue>(x, y, idx, lam, mu, max_iter, verbose);
}
