/*
  Dynamic programming solver for tree graphs.
 */
#include <iostream>
#include <stdexcept>
#include <sstream>          // std::ostringstream

#include "utils/timer.hpp"
#include "utils/minmax.hpp"

#include "dp_tree.hpp"
#include "typedefs.hpp"
#include "dp_debug.hpp"
#include "finite.hpp"
#include "clip.hpp"
#include "stack.hpp"

#include "clip2.hpp"
#include "queues.cpp"


template<typename E = Event2>
struct DPTreeStatus
{
    DPTreeStatus(const int n) :
        childs(n),
        elements_(2*n),
        pq(n),
        lb(n, 0.0) {
        proc_order.reserve(n);
        dfs_stack.reserve(n);
    }

    ChildrenIndex childs;
    stack<int> dfs_stack;

    std::vector<int> proc_order;
    std::vector<E> elements_;
    std::vector<Queue> pq;
    std::vector<double> lb;
};


template <bool merge_sort, typename E>
inline const double*
_dp_tree(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root,
    DPTreeStatus<E> &s)
{
    auto *elements = s.elements_.data();
    auto &pq = s.pq;
    auto
        *lb = s.lb.data(),
        *ub = x;
    auto *sig = lb;
    const auto &childs = s.childs;
    const auto &proc_order = s.proc_order;

    {   Timer _ ("children index");
        s.childs.reset(parent, n, root);
    }

    init_queues(n, pq, s.proc_order, childs, s.dfs_stack, root);

    {   Timer _ ("forward");
        for (auto i : proc_order) {
            sig[parent[i]] += lam;
            const auto sig_i = sig[i];  // backup before it is set in next line
            lb[i] = clip_front(elements, pq[i], mu, -mu*y[i] -sig_i, -lam);
            ub[i] = clip_back(elements, pq[i], mu, -mu*y[i] +sig_i, +lam);
            if (merge_sort)
                pq[parent[i]] = merge2(pq[parent[i]], pq[i], elements);
            else
                pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
        }
    }

    {   Timer _ ("backtrace");
        const auto r = root;
        x[r] = clip_front(elements, pq[r], mu, -mu*y[r] -sig[r], 0.0);
        for (int j = n-2; j >= 0; j--) {
            const auto v = proc_order[j];
            x[v] = clip(x[parent[v]], lb[v], ub[v]);
        }
    }

    Timer::startit("free");
    return x;
}


/**
   Paramters:
    x   Output solution (if NULL, allocate it); x == y possible.
 */
template <bool merge_sort>
const double*
dp_tree(
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
    DPTreeStatus<Event2> s(n);
    timer.stop();
    return _dp_tree<merge_sort>(n, x, y, parent, lam, mu, root, s);
}


const double*
dp_tree_weighted(
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

    double min_y, max_y;
    {   Timer _ ("minmax");
        find_minmax(y, n, min_y, max_y);
        if (n > 50 && min_y < 0.0) {
            throw std::runtime_error(std::string("y < 0: ") +
                                     std::to_string(min_y));
        }
        if (n > 50 && max_y > 1.0) {
            throw std::runtime_error(std::string("y > 1: ") +
                                     std::to_string(max_y));
        }
    }

    Timer timer ("memory alloc");
    if (x == nullptr) {
        x = new double[n];
    }
    std::vector<Event2> elements_ (2*n);
    std::vector<int> proc_order;
    ChildrenIndex childs (n);
    std::vector<Queue> pq (n);
    stack<int> stack;
    std::vector<double>
        lb (n, 0.0),
        ub (n);
    std::vector<double>
        &sig = lb;
    timer.stop();

    {   Timer _ ("children index");
        childs.reset(parent, n, root);
    }

    init_queues(n, pq, proc_order, childs, stack, root);

    auto *elements = elements_.data();
    {   Timer _ ("forward");
        for (auto i : proc_order) {
            if (parent[i] == i)
                throw std::runtime_error("Should not happen i");

            const auto lami = lam[i];
            const auto sigi = sig[i];  // backup before it is set in next line
            {   // EVENT_REC(decltype(elements[0]));
                lb[i] = clip_fronw(elements, pq[i],
                                   /* slope  */ +mu[i],
                                   /* offset */ -mu[i]*y[i] -sigi,
                                   /* t      */ -lami,
                                   /* lower_bound */ min_y);
                // EVENT_CHECK(pq[i].start, lb[i]);
            }
            {   // EVENT_REC(decltype(elements[0]));
                ub[i] = clip_backw(elements, pq[i], mu[i], -
                                   mu[i]*y[i] +sigi, +lami, max_y);
                // EVENT_CHECK(pq[i].stop, ub[i]);
                if (pq[i].start <= pq[i].stop) {
                    double lam_plus = mu[i] > 1e-10 ? lami : std::min(lami, sigi);
                    if (lam_plus > 0) {
                        sig[parent[i]] += lam_plus;
                    } else {
                        if (lam_plus < -1e-9) {
                            throw std::runtime_error("shit");
                            // EVENT_CHECK(pq[i].stop, ub[i]);
                        }
                    }
                }
            }
            pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
        }
    }

    {   Timer _ ("backtrace");
        const auto r = root;
        x[r] = clip_fronw(elements, pq[r], mu[r], -mu[r]*y[r] -sig[r], 0., min_y);
        if (x[r] > 1e10) {
            fprintf(stdout,
                    "processing %d (parent=%d): sigi = %.3f, (lam[r] = %.3f)\n",
                    r, parent[r], sig[r], lam[r]);
            throw std::runtime_error(std::string("x[r] = ") +
                                     std::to_string(x[r]));
        }
        for (int j = n-2; j >= 0; j--) {
            const auto v = proc_order[j];
            x[v] = clip(x[parent[v]], lb[v], ub[v]);
        }
    }

    Timer::startit("free");
    return x;
}


// template instantiation
template
const double*
dp_tree<true>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);


template
const double*
dp_tree<false>(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    const int root);
