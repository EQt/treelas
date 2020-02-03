#include "tree_apx.hpp"

#define PRINT_MAX 20


template<typename float_, typename int_>
void
tree_apx(
    const size_t n,
    const int_ *parent,
    const float_ *y,
    const float_ lam,
    float_ *x,
    const int_ root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder)
{
    Timer _ ("tree_apx:\n");

    std::vector<int_> porder;    // post order (forward)
    std::vector<int_> iorder;    // inverse of porder
    ChildrenIndex cidx;
    int_ root = root_;

    Timer::log("lam = %f\n", lam);

    if (root < 0) {
        Timer _ ("find_root");
        root = find_root(n, parent);
    }
    {
        Timer _ ("vector::resize");
        porder.reserve(n);
        iorder.reserve(n);
    }
    {
        Timer _ ("children idx");
        cidx.reset(n, parent, root);
    }
    //if (use_bfs)
    {
        Timer _ ("bfs");
        reversed_bfs(porder.data(), cidx);
    } /*else {
        Timer _ ("dfs postorder\n");
        stack<int_> stack;
        post_order(porder.data(), cidx, stack);
       }*/
    if (porder[n-1] != root)
        throw std::runtime_error(
            std::string("tree_apx(): FATAL: ") +
            "forder[" + std::to_string(n-1) + "] = " +
            std::to_string(porder[n-1]) + " != " +
            std::to_string(root) + " = root");

    {   Timer _ ("inverse order");
        invperm(n, iorder.data(), porder.data());
    }

    Timer tim ("alloc");
    TreeApx<float_, int_> s (n, porder.data(), reorder);
    tim.stop();

    float_ min_y, max_y, delta;
    {   Timer _ ("minmax y");
        find_minmax(y, n, min_y, max_y);
        delta = float_((max_y - min_y) * 0.5);
    }

    if (n <= PRINT_MAX) {
        printf("   parent: ");
        print_int_list(Vec(parent, n));
        printf("postorder: ");
        print_int_list(porder);
        printf("   iorder: ");
        print_int_list(iorder);
    }

    {   Timer _ ("init x,y,parent");
#ifdef DEBUG_ID
        s.id.resize(n);
#endif
        const float_ x0 = float_(0.5 * (min_y + max_y));
        for (size_t i = 0; i < n; i++)
            s.x[i] = x0;
        if (reorder) {
            for (size_t i = 0; i < n; i++) {
                const auto ii = porder[i];
#ifdef DEBUG_ID
                s.id[i] = ii;
#endif
                s.y[i] = y[ii];
                s.init_parent(i, iorder[parent[ii]]);
            }
        } else {
            for (size_t i = 0; i < n; i++) {
#ifdef DEBUG_ID
                s.id[i] = int(i);
#endif
                s.y[i] = y[i];
                s.init_parent(i, parent[i]);
            }
        }
    }
    if (n <= PRINT_MAX) {
        printf("deriv: [");
        for (size_t i = 0; i < n; i++)
            printf("%.3f ", s.deriv[reorder ? iorder[i] : i]);
        printf("]\n");
        printf("    x: [");
        for (size_t i = 0; i < n; i++)
            printf("%.3f ", s.x[reorder ? iorder[i] : i]);
        printf("]\n");
    }
    {   Timer _ ("iterations:\n");
        for (int k = 0; k < max_iter; k++) {
            size_t changed = 0;
            Timer::log("%2ld ...%s", k+1, print_timings ? "\n" : "");

            delta = float_(0.5*delta);
            {
                TimerQuiet _ (print_timings);
                changed = s.iter(lam, delta);
                if (n <= PRINT_MAX) {
                    printf("deriv: [");
                    for (size_t i = 0; i < n; i++)
                        printf("%.3f ", s.deriv[reorder ? iorder[i] : i]);
                    printf("]\n");
                    printf("    x: [");
                    for (size_t i = 0; i < n; i++)
                        printf("%.3f ", s.x[reorder ? iorder[i] : i]);
                    printf("]\n");
                }
            }
            if (changed)
                Timer::log("  %'d", changed);
            Timer::log("\n");
        }
    }

    {   Timer _ ("extract x");
        if (reorder) {
            for (size_t i = 0; i < n; i++) {
                x[i] = s.x[iorder[i]];
            }
        } else {
            for (size_t i = 0; i < n; i++)
                x[i] = s.x[i];
        }
    }
}


template
void
tree_apx(
    const size_t n,
    const int *parent,
    const float *y,
    const float lam,
    float *x,
    const int root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder);


template
void
tree_apx(
    const size_t n,
    const int *parent,
    const double *y,
    const double lam,
    double *x,
    const int root_,
    const int max_iter,
    const bool print_timings,
    const bool reorder);
