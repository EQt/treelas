/**
   Dynamic programming optimal solver for fused lasso on trees.
 */
#pragma once

#include <iostream>
#include <stdexcept>
#include <limits>

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/weights.hpp>
#include <graphidx/std/stack.hpp>
#include <graphidx/std/uvector.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/timer.hpp>


#include "clip.hpp"
#include "merge.hpp"


struct TreeDPStatus
{
    TreeDPStatus(const size_t n) :
        childs(n) {
        lb.reserve(n);
        elements_.reserve(2*n),
        pq.reserve(n),
        proc_order.reserve(n);
        dfs_stack.reserve(n);
    }

    ChildrenIndex childs;
    stack<int> dfs_stack;

    std::vector<int> proc_order;
    uvector<Event> elements_;
    uvector<Range> pq;
    uvector<double> lb;
};


template <bool merge_sort, bool lazy_sort, typename Wlam, typename Wmu>
inline const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const Wlam &lam,
    const Wmu &mu,
    const int root,
    TreeDPStatus &s)
{
    if (root < 0) {
        int new_root = -1;
        {
            Timer _ ("find root");
            new_root = find_root(n, parent);
        }
        return tree_dp<merge_sort, lazy_sort>(n, x, y, parent, lam, mu, new_root, s);
    }

    auto *elements = s.elements_.data();
    auto &pq = s.pq;
    auto
        *lb = s.lb.data(),
        *ub = x;
    auto *sig = lb;
    const auto &childs = s.childs;
    const auto &proc_order = s.proc_order;
    constexpr bool check = !Wmu::is_const();

    {   Timer _ ("lb init");
        std::fill(lb, lb + n, 0);
    }
    {   Timer _ ("children index");
        s.childs.reset(n, parent, root);
    }

    init_queues(n, pq, s.proc_order, childs, s.dfs_stack, root);
    {   Timer _ ("forward");
        for (auto i : proc_order) {
            const auto sig_i = sig[i];  // backup before it is set in next line
            if (!merge_sort && lazy_sort)
                sort_events(pq[i], elements);
            lb[i] = clip<+1, check>(elements, pq[i], +mu[i], -mu[i]*y[i] - sig_i + lam[i]);
            ub[i] = clip<-1, check>(elements, pq[i], -mu[i], +mu[i]*y[i] - sig_i + lam[i]);
            sig[parent[i]] +=
                (check && mu[i] <= EPS) ? std::min(lam[i], sig_i) : lam[i];
            if (merge_sort)
                pq[parent[i]] = merge2(pq[parent[i]], pq[i], elements);
            else {
                pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
                if (!lazy_sort)
                    sort_events(pq[parent[i]], elements);
            }
        }
    }

    {   Timer _ ("backtrace");
        const auto r = root;
        if (!merge_sort && lazy_sort)
            sort_events(pq[r], elements);
        x[r] = clip<+1, check>(elements, pq[r],
                               +mu[r], -mu[r]*y[r] -sig[r] + 0.0);
        for (long int j = (long int)(n-2); j >= 0; j--) {
            const auto v = proc_order[j];
            x[v] = clamp(x[parent[v]], lb[v], ub[v]);
        }
    }

    Timer::startit("free");
    return x;
}


/**
   Paramters:
    x   Output solution (if NULL, allocate it); x == y possible.
 */
template <bool merge_sort, bool lazy_sort, typename Wlam, typename Wmu>
inline const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const Wlam &lam,
    const Wmu &mu,
    const int root)
{
    Timer timer ("memory alloc");
    if (x == nullptr)
        x = new double[n];
    TreeDPStatus s(n);
    timer.stop();
    return tree_dp<merge_sort, lazy_sort, Wlam, Wmu>(
        n, x, y, parent, lam, mu, root, s);
}


template <bool merge_sort = true, bool lazy_sort = false>
inline
std::vector<double>
tree_dp(
    const std::vector<double> &y,
    const std::vector<int> &parent,
    const double lam,
    const int root = 0)
{
    std::vector<double> x;
    const size_t n = y.size();
    x.resize(n);
    tree_dp<merge_sort, lazy_sort>(
        n,
        x.data(),
        y.data(),
        parent.data(),
        Const<double>(lam),
        Ones<double>(),
        root);
    return x;
}


template <bool merge_sort, bool lazy_sort = false>
const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu = 1.0,
    const int root = 0);


extern template
const double*
tree_dp<true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


extern template
const double*
tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


extern template
const double*
tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template <bool lazy_sort, bool merge_sort = true>
[[deprecated]]
const double*
tree_dp_w(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root = 0);
