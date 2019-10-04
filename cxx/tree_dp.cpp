/*
  Dynamic programming solver for tree graphs.
 */
#include <iostream>
#include <stdexcept>
#include <limits>

#include <graphidx/bits/clamp.hpp>
#include <graphidx/bits/finite.hpp>
#include <graphidx/bits/minmax.hpp>
#include <graphidx/std/stack.hpp>
#include <graphidx/utils/timer.hpp>

// #include "tree_debug.hpp"
#include "clip.hpp"
#include "merge.hpp"


template<typename E = Event>
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
    std::vector<E> elements_;
    std::vector<Range> pq;
    std::vector<double> lb;
};


template <bool merge_sort, bool lazy_sort, typename E>
inline const double*
tree_dp(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root,
    TreeDPStatus<E> &s)
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
            sig[parent[i]] += lam;
            const auto sig_i = sig[i];  // backup before it is set in next line
            if (!merge_sort && lazy_sort)
                sort_events(pq[i], elements);
            lb[i] = clip_front(elements, pq[i], mu, -mu*y[i] -sig_i + lam);
            ub[i] = clip_back(elements, pq[i], mu, -mu*y[i] +sig_i, +lam);
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
        x[r] = clip_front(elements, pq[r], mu, -mu*y[r] -sig[r] + 0.0);
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
    TreeDPStatus<Event> s(n);
    timer.stop();
    return tree_dp<merge_sort, lazy_sort>(n, x, y, parent, lam, mu, root, s);
}


template <bool lazy_sort>
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

    const double
        min_y = -std::numeric_limits<double>::infinity(),
        max_y = +std::numeric_limits<double>::infinity();

    Timer timer ("memory alloc");
    if (x == nullptr) {
        x = new double[n];
    }
    std::vector<Event> elements_ (2*n);
    std::vector<int> proc_order;
    ChildrenIndex childs (n);
    std::vector<Range> pq (n);
    stack<int> stack;
    std::vector<double>
        lb (n, 0.0),
        ub (n);
    std::vector<double>
        &sig = lb;
    timer.stop();

    {   Timer _ ("children index");
        childs.reset(n, parent, root);
    }

    init_queues(n, pq, proc_order, childs, stack, root);

    auto *elements = elements_.data();
    {   Timer _ ("forward");
        for (auto i : proc_order) {
            if (parent[i] == i)
                throw std::runtime_error(
                    std::string("tree_dp_w(): FATAL ERROR: ") +
                    "parent[" + std::to_string(i) + "] = " +
                    std::to_string(parent[i]));

            const auto lami = lam[i];
            const auto sigi = sig[i];  // backup before it is set in next line
            if (lazy_sort)
                sort_events(pq[i], elements);
            lb[i] = clip_fronw(elements, pq[i],
                               /* slope  */ +mu[i],
                               /* offset */ -mu[i]*y[i] -sigi,
                               /* t      */ -lami,
                               /* lower_bound */ min_y);
            ub[i] = clip_backw(elements, pq[i], mu[i], -
                               mu[i]*y[i] +sigi, +lami, max_y);
            sig[parent[i]] += mu[i] > 1e-10 ? lami : std::min(lami, sigi);
            pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
            if (!lazy_sort)
                sort_events(pq[parent[i]], elements);
        }
    }

    {   Timer _ ("backtrace");
        const auto r = root;
        if (lazy_sort)
            sort_events(pq[r], elements);
        x[r] = clip_fronw(elements, pq[r], mu[r], -mu[r]*y[r] -sig[r], 0., min_y);
        if (x[r] > 1e10) {
            fprintf(stdout,
                    "processing %d (parent=%d): sigi = %.3f, (lam[r] = %.3f)\n",
                    r, parent[r], sig[r], lam[r]);
            throw std::runtime_error(std::string("x[r] = ") +
                                     std::to_string(x[r]));
        }
        for (long int j = (long int)(n-2); j >= 0; j--) {
            const auto v = proc_order[j];
            x[v] = clamp(x[parent[v]], lb[v], ub[v]);
        }
    }

    Timer::startit("free");
    return x;
}


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


template
const double*
tree_dp_w<true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root);


template
const double*
tree_dp_w<false>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double *lam,
    const double *mu,
    const int root);
