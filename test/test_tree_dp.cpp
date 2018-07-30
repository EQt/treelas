#include <gtest/gtest.h>

#include "../cxx/utils/timer.hpp"       // TimerQuiet
#include "../cxx/utils/plotpwl.hpp"
#include "../cxx/utils/minmax.hpp"      // find_minmax

#include "../cxx/tree_dp.hpp"
#include "../cxx/queue.hpp"
#include "../cxx/event.hpp"
#include "../cxx/stack.hpp"
#include "../cxx/clip.hpp"

#include "../cxx/queue.cpp"


#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wpedantic"


/* Test data generated by:
   ```julia
   include("test_cleantree.jl")
   TestFLSA2.compare(5)
   pq, proc_order, _ = FLSA2.init_queues(TestFLSA2.parent);
   println("{", join(["{$(p.start-1), $(p.stop-1)}" for p in pq], ",\n "), "}")
   println(proc_order-1)
   ```
*/
TEST(dptree, init_queues)
{
    TimerQuiet _;

    const std::vector<int> parent = {0, 0, 1, 2, 3, 0, 7, 8, 3, 8};
    const int n = parent.size();
    std::vector<Queue> pq (n);
    ChildrenIndex childs (parent);
    stack<int> stack;
    std::vector<int> proc_order;

    init_queues(n, pq, proc_order, childs, stack);

    const std::vector<Queue> pq_expect {
        {21, 20},
        {20, 19},
        {19, 18},
        {18, 17},
        {17, 16},
        {4,   3},
        {13, 12},
        {14, 13},
        {15, 14},
        {10,  9}};
    for (int i = 0; i < n; i++) {
        EXPECT_EQ(pq[i].start, pq_expect[i].start -1);
        EXPECT_EQ(pq[i].stop, pq_expect[i].stop -1);
        // if (pq[i].start != pq_expect[i].start ||
        //     pq[i].stop != pq_expect[i].stop) {
        //     std::cerr << __FILE__ << ":" << __LINE__ << ": "
        //               << i << ": interval " << pq[i] << " vs " << pq_expect[i]
        //               << std::endl;
        // }
    }

    const std::vector<int> proc_order_expect {5, 9, 6, 7, 8, 4, 3, 2, 1};
    EXPECT_EQ(proc_order, proc_order_expect);
    // for (int i = 0; i < n-1; i++) {
    //     EXPECT_EQ(proc_order[i], proc_order_expect[i]);
    //     // if (proc_order[i] != proc_order_expect[i]) {
    //     //     std::cerr << __FILE__ << ":" << __LINE__ << ": "
    //     //               << i << ": proc_order " << proc_order[i]
    //     //               << " vs " << proc_order_expect[i]
    //     //               << std::endl;
    //     //     std::cerr << proc_order << std::endl << "vs" << std::endl
    //     //               << proc_order_expect << std::endl;
    //     //     break;
    //     // }
    // }
}


/* inspired by `tiny3_test_merge2` in treequeue.jl
   ```julia
   println("{", join(map(string, round.(elements, 2)), ",\n "), "}")
   ```
 */
TEST(dptree, merge)
{
    Queue
        parent {1, 3},
        child {8, 9};
    std::vector<double> elements =
        {0.0, 0.2, 0.22, 0.27, 0.0, 0.0, 0.0, 0.0, 0.48, 0.68, 0.0};

    const Queue res = merge(parent, child, elements);
    // std::cerr << res << " vs " << Queue({1,5}) << std::endl;
    EXPECT_EQ(res, Queue({1,5}));
    std::vector<double> elements_expect =
        {0.0, 0.2, 0.22, 0.27, 0.48, 0.68, 0.0, 0.0, 0.48, 0.68, 0.0};

    // std::cerr << elements << std::endl << "vs" << std::endl
    //           << elements_expect << std::endl;
    EXPECT_EQ(elements, elements_expect);
}


TEST(dptree0, DISABLED_proc_order)
{
    /*
                              ┌───┐
                 ┌──────────▶ │ 0 │ ◀──────────┐
                 │            └───┘            │
                 │                             │
     ┌───┐     ┌───┐     ┌───┐     ┌───┐     ┌───┐     ┌───┐
     │ 6 │ ──▶ │ 2 │ ◀── │ 5 │     │ 4 │ ──▶ │ 1 │ ◀── │ 3 │
     └───┘     └───┘     └───┘     └───┘     └───┘     └───┘

    */
    TimerQuiet _;

    const std::vector<int> parent = {0, 0, 0, 1, 1, 2, 2};
    const int n = 7;
    ASSERT_EQ(parent.size(), n);
    std::vector<Queue> pq (n);
    std::vector<int> proc_order;
    ChildrenIndex childs (parent);
    stack<int> stack;
    init_queues(n, pq, proc_order, childs, stack);

    const std::vector<int> proc_order_expect {6, 5, 2, 4, 3, 1};
    ASSERT_EQ(proc_order, proc_order_expect);

    // const std::vector<int> numbers = {1, 3, 0, 2};
    const std::vector<double> y   = {0.0, 0.0, 0.0, 1.0, 3.0, 0.0, 2.0};
    const std::vector<double> mu  = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    const std::vector<double> lam = {0.04, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01};
    ASSERT_EQ(y.size(), n);
    ASSERT_EQ(mu.size(), n);
    ASSERT_EQ(lam.size(), n);

    double min_y, max_y;
    find_minmax(y.data(), n, min_y, max_y);
    ASSERT_EQ(min_y, 0.0);
    ASSERT_EQ(max_y, 3.0);

    std::vector<double> lb (n, 0), ub (n);
    std::vector<double>  &sig = lb;
    std::vector<Event> elements_ (2*n);
    Event *elements = elements_.data();

    ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0, 0, 0, 0, 0}));
    {   int i = proc_order[0];
        ASSERT_EQ(i, 6);
        ASSERT_EQ(parent[i], 2);
        ASSERT_EQ(pq[i], Queue({4, 3}));
        const auto lam_i = lam[i];
        sig[parent[i]] += lam_i;
        ASSERT_DOUBLE_EQ(lam_i, 0.01);
        ASSERT_DOUBLE_EQ(sig[parent[i]], 0.01);
        ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0.01, 0, 0, 0, 0}));
        const auto sig_i = sig[i];
        lb[i] = clip_fronw(elements, pq[i],
                           /* slope  */ +mu[i],
                           /* offset */ -mu[i]*y[i] -sig_i,
                           /* t      */ -lam_i, min_y);
        ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0.01, 0, 0, 0, 1.99}));
        ASSERT_DOUBLE_EQ(lb[i], 2.0 - 0.01);
        ASSERT_EQ(pq[i], Queue({3, 3}));
        ASSERT_EQ(elements[3],
                  Event({.x = lb[i], .slope = mu[i],
                         .offset = -mu[i]*y[i] + lam[i]}));

        ub[i] = clip_backw (elements, pq[i], mu[i], -mu[i]*y[i] +sig_i, +lam_i, max_y);
        ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0.01, 0, 0, 0, 1.99}));
        ASSERT_DOUBLE_EQ(ub[i], 2.0 + 0.01);
        ASSERT_EQ(pq[i], Queue({3, 4}));
        ASSERT_EQ(elements[4],
                  Event({.x = ub[i], .slope = -mu[i],
                         .offset = +mu[i]*y[i] + lam[i]}));

        ASSERT_EQ(pq[parent[i]], Queue({7, 6}));
        pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
        ASSERT_EQ(pq[parent[i]], Queue({3, 4}));
    }
    {   int i = proc_order[1];
        ASSERT_EQ(i, 5);
        ASSERT_EQ(parent[i], 2);
        ASSERT_EQ(pq[i], Queue({6, 5}));
        const auto lam_i = lam[i];
        ASSERT_DOUBLE_EQ(lam_i, 0.01);
        sig[parent[i]] += lam_i;
        const auto sig_i = sig[i];
        ASSERT_DOUBLE_EQ(sig[parent[i]], 0.02);
        ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0.02, 0, 0, 0, 1.99}));
        ASSERT_DOUBLE_EQ(sig_i, 0.0);
        lb[i] = clip_fronw(elements, pq[i],
                           /* slope  */ +mu[i],
                           /* offset */ -mu[i]*y[i] -sig_i,
                           /* t      */ -lam_i, min_y);
        ASSERT_DOUBLE_EQ(lb[i], 0.0 - 0.01);
        ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0.02, 0, 0, -0.01, 1.99}));
        ASSERT_EQ(pq[i], Queue({5, 5}));
        ASSERT_EQ(elements[5],
                  Event({.x = lb[i], .slope = mu[i],
                         .offset = -mu[i]*y[i] + lam[i]}));

        ub[i] = clip_backw(elements, pq[i], mu[i], -mu[i]*y[i] +sig_i, +lam_i, max_y);
        ASSERT_DOUBLE_EQ(ub[i], 0.0 + 0.01);
        ASSERT_EQ(pq[i], Queue({5, 6}));
        ASSERT_EQ(elements[6],
                  Event({.x = ub[i], .slope = -mu[i],
                         .offset = +mu[i]*y[i] + lam[i]}));

        ASSERT_EQ(pq[parent[i]], Queue({3, 4}));
        pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
        ASSERT_EQ(pq[parent[i]], Queue({3, 6}));
    }
    {   int i = proc_order[2];
        ASSERT_EQ(i, 2);
        ASSERT_EQ(parent[i], 0);
        ASSERT_EQ(pq[i], Queue({3, 6}));
        const auto lam_i = lam[i];
        ASSERT_DOUBLE_EQ(lam_i, 0.02);
        ASSERT_EQ(sig, std::vector<double>({0.0, 0, 0.02, 0, 0, -0.01, 1.99}));
        ASSERT_DOUBLE_EQ(sig[parent[i]], 0.0);
        sig[parent[i]] += lam_i;
        ASSERT_EQ(sig, std::vector<double>({0.02, 0, 0.02, 0, 0, -0.01, 1.99}));
        const auto sig_i = sig[i];
        ASSERT_DOUBLE_EQ(sig_i, 0.02);
        ASSERT_EQ(std::vector<double>({-0.01, +0.01, +1.99, +2.01}),
                  std::vector<double>({elements[3].x, elements[4].x,
                                       elements[5].x, elements[6].x}));
        {
            Queue &q = pq[i];
            double
                slope = +mu[i],
                offset = -mu[i]*y[i] -sig_i,
                t = -lam_i;
            auto &e = elements[q.start];
            ASSERT_EQ(offset, -0.02);
            ASSERT_EQ(slope, 0.0);
            ASSERT_EQ(e, Event({.x = -0.01, .slope = 1, .offset=+0.01}));
            ASSERT_EQ(slope * e.x + offset, -0.02);
            ASSERT_EQ(t, -0.02);
            ASSERT_EQ(slope * e.x + offset, t);
            ASSERT_LE(q.start, q.stop);
            lb[i] = clip_fronw(elements, pq[i],
                               /* slope  */ slope,
                               /* offset */ offset,
                               /* t      */ t, min_y);
            ASSERT_EQ(lb[i], 0.0 /*-0.01*/);    // uncertainty...
            ASSERT_EQ(offset - t, 0.0);
            ASSERT_EQ(pq[i], Queue({3, 6}));
            // TODO: Think about merging those elments automatically
            ASSERT_EQ(elements[2],
                      Event({.x = -0.01, .slope = 0.0, .offset = 0.0}));
            ASSERT_EQ(elements[3],
                      Event({.x = -0.01, .slope = 1.0, .offset = 0.01}));
            EXPECT_EQ(pq[i].start, 2) << pwl_csv(pq[i], elements) << std::endl;
        }
        {
            Queue &q = pq[i];
            ASSERT_LE(q.start, q.stop);
            auto &e = elements[q.stop];
            double
                slope = +mu[i],
                offset = -mu[i]*y[i] + sig_i,
                t = +lam_i;
            ASSERT_EQ(offset, +0.02);
            ASSERT_EQ(slope,  +0.0);
            ASSERT_EQ(e, Event({.x = +2.01, .slope = -1, .offset = +2.01}));
            ASSERT_EQ(slope * e.x + offset, +0.02);
            ASSERT_EQ(t, +0.02);
            ASSERT_EQ(slope * e.x + offset, t);

            ub[i] = clip_backw (elements, q, slope, offset, t, max_y);
            ASSERT_DOUBLE_EQ(ub[i], 2.0 + 0.01);
            ASSERT_EQ(offset - t, 0.0);
            ASSERT_EQ(pq[i], Queue({2, 7}));
            ASSERT_EQ(elements[7],
                      Event({.x = +2.01, .slope = +0.0, .offset = +0.00}));
            ASSERT_EQ(elements[6],
                      Event({.x = +2.01, .slope = -1.0, .offset = +2.01}));
        }

        pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
        ASSERT_EQ(pq[parent[i]], Queue({2, 7}));
    }
    {
        const int i = proc_order[3];
        ASSERT_EQ(i, 4);
        const auto lam_i = lam[i];
        sig[parent[i]] += lam_i;
        const auto sig_i = sig[i];  // backup before it is set in next line
        lb[i] = clip_fronw(elements, pq[i], mu[i], -mu[i]*y[i] -sig_i, -lam_i, min_y);
        ub[i] = clip_backw (elements, pq[i], mu[i], -mu[i]*y[i] +sig_i, +lam_i, max_y);
        pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
    }
    {
        const int i = proc_order[4];
        ASSERT_EQ(i, 3);
        const auto lam_i = lam[i];
        sig[parent[i]] += lam_i;
        const auto sig_i = sig[i];  // backup before it is set in next line
        lb[i] = clip_fronw(elements, pq[i], mu[i], -mu[i]*y[i] -sig_i, -lam_i, min_y);
        ub[i] = clip_backw(elements, pq[i], mu[i], -mu[i]*y[i] +sig_i, +lam_i, max_y);
        pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
    }
    {
        const int i = proc_order[5];
        ASSERT_EQ(i, 1);
        const auto lam_i = lam[i];
        const auto sig_i = sig[i];  // backup before it is set in next line
        sig[parent[i]] += lam_i;
        lb[i] = clip_fronw(elements, pq[i], mu[i], -mu[i]*y[i] -sig_i, -lam_i, min_y);
        ub[i] = clip_backw(elements, pq[i], mu[i], -mu[i]*y[i] +sig_i, +lam_i, max_y);
        pq[parent[i]] = merge(pq[parent[i]], pq[i], elements);
    }
    {   // final merge
        ASSERT_EQ(pq[0], Queue({2, 13}));
        ASSERT_EQ(pwl_csv(pq[0], elements),
                  R"(-1.01,-0.04
-0.01,-0.04
-0.01,-0.04
+0.01,-0.02
+0.99,-0.02
+0.99,-0.02
+1.01,+0
+1.99,+0
+2.01,+0.02
+2.01,+0.02
+2.99,+0.02
+3.01,+0.04
+3.01,+0.04
+4.01,+0.04
)"
            );
    }
    {   // check bounds
        ASSERT_EQ(lb[1], +0.99);
        ASSERT_EQ(ub[1], +3.01);
        ASSERT_EQ(lb[2], -0.01);
        ASSERT_EQ(ub[2], +2.01);

        for (int i = 3; i < 6; i++) {
            ASSERT_EQ(lb[i], y[i] - lam[i]) << i;
            ASSERT_EQ(ub[i], y[i] + lam[i]) << i;
        }
    }
    {   // find root value
    }
}

#pragma GCC diagnostic pop
