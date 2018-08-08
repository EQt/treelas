/*
  Discrete approximation of FLSA on tree graphs.

  This implementation evaluates the derivative at one point per
  iteration, yielding the better option of two possible choices for
  each x[i] (hence the name `tree12x`).
*/
#include <cmath>        // for std::abs
#include <vector>
#include <algorithm>    // for std::min_element

#include "../utils/hdf5.hpp"
#include "../utils/timer.hpp"
#include "../utils/argparser.hpp"
#include "../utils/thousand.hpp"
#include "../utils/viostream.hpp"
#include "../utils/minmax.hpp"

#include "../root.hpp"
#include "../postorder.hpp"



template<typename float_ = float, typename int_ = int>
struct Tree12xStatus
{
    Tree12xStatus(const size_t n, const int_ *forder)
        : n(n),
          forder(forder) {
        y = new float_[n];
        x = new float_[n];
        deriv = new float_[n];
        parent_ = new int_[n];
    }

    static constexpr int_ one =             //  b"100...0"
        (int_(1)<<(8*sizeof(int_)-1));

    inline void divorce(size_t i) { parent_[i] &= ~one; }
    inline void init_parent(size_t i, int_ p) { parent_[i] = p | one; }
    inline bool same(size_t i) { return bool(parent_[i] & one); }
    inline int_ parent(size_t i) { return parent_[i] & (~one); }

    ~Tree12xStatus() {
        if (y) delete[] y;
        if (x) delete[] x;
        if (deriv) delete[] deriv;
        if (parent_) delete[] parent_;
    }

    const size_t n = 0;
    float_ *y = nullptr;
    float_ *x = nullptr;
    float_ *deriv = nullptr;
    int_ *parent_ = nullptr;
    const int_ *forder;
};


template<typename float_ = float, typename int_ = int>
size_t
tree_12x_iter(Tree12xStatus<float_, int_> &s, const float_ lam, const float_ delta)
{
    const auto root = s.forder[s.n-1];

    {   // Timer _ ("deriv init");
        for (size_t i = 0; i < s.n; i++)
            s.deriv[i] = s.x[i] - s.y[i];
    }

    {   // Timer _ ("forward");
        for (size_t i = 0; i < s.n-1; i++) {
            const auto v = s.forder[i];
            if (s.same(v)) {
                const auto p = s.parent(v);
                // printf("v = %d (p = %d)\n", v, p);
                s.deriv[p] += clip(s.deriv[v], -lam, +lam);
            }
        }
    }

    size_t changed = 0;
    {   // Timer _ ("backward");
        const auto xr = s.deriv[root] > 0 ? -delta : +delta;
        // printf("\nxr = %f\n", xr);
        s.x[root] += xr;

        Timer::log("delta = %f\n", delta);
        Timer::log("x[0] = %f\n", s.x[root]);
        for (size_t i = s.n-1; i > 0; i--) {
            const auto v = s.forder[i-1];
            // printf("v = %d  (p = %d)\n", v, s.parent(v));
            if (s.same(v)) {
                if (s.deriv[v] > lam) {
                    s.x[v] -= delta;
                } else if (s.deriv[v] < -lam) {
                    s.x[v] += delta;
                } else {
                    s.x[v] = s.x[s.parent(v)];
                    continue;
                }

                const auto p = s.parent(v);
                if (s.x[v] < s.x[p]) {
                    changed++;
                    s.divorce(v);
                    s.y[v] += lam;
                    s.y[p] -= lam;
                } else if (s.x[v] > s.x[p]) {
                    changed++;
                    s.divorce(v);
                    s.y[v] -= lam;
                    s.y[p] += lam;
                }
            } else {
                s.x[v] += s.deriv[v] < 0 ? +delta : -delta;
            }
        }
    }

    return changed;
}


template<typename float_ = float, typename int_ = int>
void
tree_12x(
    const size_t n,
    const int_ *parent,
    const float_ *y,
    const float_ lam,
    float_ *x,
    const int_ root_ = int_(-1),
    const int max_iter = 3)
{
    std::vector<int> forder_;
    forder_.reserve(n);
    int_ root = root_ < 0 ? find_root(n, parent) : root_;

    {   Timer _ ("dfs postorder\n");
        post_order(n, parent, root, forder_.data());
        if (forder_[n-1] != root)
            throw std::runtime_error(std::string("tree_12x(): FATAL: ") +
                                     "forder[" + std::to_string(n-1) + "] = " +
                                     std::to_string(forder_[n-1]) + " != " +
                                     std::to_string(root) + " = root");
    }

    Timer tim ("alloc");
    Tree12xStatus<float_, int_> s (n, forder_.data());
    tim.stop();

    float_ min_y, max_y, delta;
    {   Timer _ ("minmax y");
        find_minmax(y, n, min_y, max_y);
        delta = float_((max_y - min_y) * 0.25);
    }

    {   Timer _ ("init x,y,parent");
        const float_ x0 = float_(0.5 * (min_y + max_y));
        for (size_t i = 0; i < n; i++)
            s.x[i] = x0;
        for (size_t i = 0; i < n; i++)
            s.y[i] = y[i];
        for (size_t i = 0; i < n; i++) {
            s.init_parent(i, parent[i]);
            printf("parent[%d] = %d\n", int(i), s.parent(i));
        }
    }

    for (int k = 0; k < max_iter; k++) {
        Timer::log("%2ld ...", k+1);
        const auto changed = tree_12x_iter(s, lam, delta);
        if (changed)
            Timer::log("  %d", changed);
        Timer::log("\n");

        delta = 0.5*delta;
    }


    {   Timer _ ("extract x");
        for (size_t i = 0; i < n; i++)
            x[i] = s.x[i];
    }
}


template<typename float_ = float, typename int_ = int>
void
process_file(const char *fname,
             const char *group,
             const int max_iter,
             const bool /*use_dfs*/,
             const unsigned PRINT_MAX = 10)
{
    std::vector<double> xt, y, lam, x;
    std::vector<int> parent;
    {   // Read extra information
        HDF5 io (fname, "r+");
        io.group(group);

        io.readv(y,      "y");
        io.readv(lam,   "lam");
        io.readv(parent, "parent");

        if (io.has("xt")) {
            xt = io.read<double>("xt");
        } else if (io.has("x++")) {
            xt = io.read<double>("x++");
        }
    }
    const size_t n = parent.size();
    if (y.size() != n)
        throw std::runtime_error(std::string("y.size() = ") + std::to_string(y.size()) +
                                             " != " + std::to_string(n));
    x.resize(n);

    tree_12x(n,
             parent.data(),
             y.data(),
             lam[0],
             x.data(),
             -1 /* root*/,
             max_iter);

    if (n <= PRINT_MAX) {
        fprintf(stdout, " x: ");
        print(x, 5, stdout);
        if (xt.size() == n) {
            fprintf(stdout, "xt: ");
            print(xt, 5, stdout);
        }
    }

    if (xt.size() == n) {
        double max_diff = 0.0;
        for (unsigned i = 0; i < n; i++)
            max_diff = std::max(max_diff, std::abs(x[i] - xt[i]));

        fprintf(stdout, "Norm(x - xt, Inf):  %g\n", max_diff);
    }
}


int
main(int argc, char *argv[])
{
    try {
        set_thousand_sep(std::cout, '\'');

        ArgParser ap ("tree_12x [file]\n");
        ap.add_option('i', "max-iter",
                      "Number of iterations [default 3]",
                      "INT", "3");
        ap.add_option('d', "dfs",     "Use DFS instead of BFS order");
        ap.add_option('6', "float64", "Calculate in float64_t precision");
        ap.parse(&argc, argv);
        if (argc <= 1) {
            fprintf(stderr, "No tree file!\n");
            return 1;
        }
        const char *fname = argv[1];
        const int max_iter = atoi(ap.get_option("max-iter"));
        typedef int   int_;
        const char *group = "/";
        printf("max_iter = %d\n", max_iter);
        if (ap.has_option("float64")) {
            process_file<double, int_>(fname, group,
                                       max_iter,
                                       ap.has_option("dfs"));
        } else {
            process_file<float, int_>(fname, group,
                                       max_iter,
                                       ap.has_option("dfs"));
        }
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    }
    return 0;
}
