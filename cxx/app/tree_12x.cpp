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

#include "../root.hpp"
#include "../postorder.hpp"



template<typename float_ = float, typename int_ = int>
struct Tree12xStatus
{
    Tree12xStatus(const size_t n) : n(n) {
        y = new float_[n];
        deriv = new float_[n];
        parent = new int_[n];
    }

    ~Tree12xStatus() {
        if (y) delete[] y;
        if (deriv) delete[] deriv;
        if (parent) delete[] parent;
    }

    const size_t n = 0;
    float_ *y = nullptr;
    float_ *deriv = nullptr;
    int_ *parent = nullptr;
};


template<typename float_ = float, typename int_ = int>
size_t
tree_12x_iter(Tree12xStatus<float_, int_> *s)
{
    return 1;
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
    std::vector<int> forder;

    Timer tim ("alloc");
    Tree12xStatus<float_, int_> s (n);
    forder.reserve(n);
    tim.stop();

    int_ root = root_ < 0 ? find_root(n, parent) : root_;

    post_order(n, parent, root, forder.data());

    for (int k = 0; k < max_iter; k++) {
        tree_12x_iter(&s);
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
        print(x, 3, stdout);
        if (xt.size() == n) {
            fprintf(stdout, "xt: ");
            print(xt, 3, stdout);
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
