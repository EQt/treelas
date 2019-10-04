/*
  Dynamic programming solver for tree graphs.
 */
#include <iostream>
#include <stdexcept>
#include <limits>

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/finite.hpp>
#include <graphidx/bits/minmax.hpp>
#include <graphidx/bits/weights.hpp>
#include <graphidx/std/stack.hpp>
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
    std::vector<Event> elements_;
    std::vector<Range> pq;
    std::vector<double> lb;
};


template <bool merge_sort,
          bool lazy_sort,
          bool check = true,
          typename Wlam,
          typename Wmu>
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
    auto *elements = s.elements_.data();
    auto &pq = s.pq;
    auto
        *lb = s.lb.data(),
        *ub = x;
    auto *sig = lb;
    const auto &childs = s.childs;
    const auto &proc_order = s.proc_order;

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
            sig[parent[i]] += (check && mu[i] <= EPS) ? std::min(lam[i], sig_i) : lam[i];
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
        x[r] = clip<+1, check>(elements, pq[r], mu[r], -mu[r]*y[r] -sig[r] + 0.0);
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
template <bool merge_sort, bool lazy_sort>
const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root)
{
    Timer timer ("memory alloc");
    if (x == nullptr)
        x = new double[n];
    TreeDPStatus s(n);
    timer.stop();
    ConstantWeights<double> _lam (lam);
    ConstantWeights<double> _mu (mu);
    return tree_dp<merge_sort, lazy_sort, false>(n, x, y, parent, _lam, _mu, root, s);
}


template <bool lazy_sort,
          bool merge_sort>
const double*
tree_dp_w(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root)
{
    {   Timer _ ("check finite");
        check_all_finite(y,  n, "y");
        check_all_finite(mu, n, "mu");
        const size_t i = find_non_finite(lam, n);
        if (i < n && int(i) != root) {
            throw std::runtime_error(std::string("lam[") + std::to_string(i) +
                                     "] = " + std::to_string(lam[i]) +
                                     " ; root = " + std::to_string(root));
        }
    }

    if (parent[root] != root) {     // verify root property
        throw std::invalid_argument(std::string("parent[") +
                                    std::to_string(root) + "] = " +
                                    std::to_string(parent[root]) + " != root");
    }

    Timer timer ("memory alloc");
    TreeDPStatus s(n);
    timer.stop();

    ArrayWeights<double> _lam(lam);
    ArrayWeights<double> _mu(mu);
    return tree_dp<merge_sort, lazy_sort, true>(n, x, y, parent, _lam, _mu, root, s);

    Timer::startit("free");
    return x;
}


template
double const*
tree_dp_w<true, true>(
    size_t,
    double*,
    double const*,
    int const*,
    double const*,
    double const*,
    int);


template
double const*
tree_dp_w<false, true>(
    size_t,
    double*,
    double const*,
    int const*,
    double const*,
    double const*,
    int);


// template instantiation
template
const double*
tree_dp<true, false>(    // tree_dp<true, true> does not make sense
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template
const double*
tree_dp<false, true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template
const double*
tree_dp<false, false>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);
