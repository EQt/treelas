/*
  Discrete approximation of FLSA on tree graphs.

  This implementation evaluates the derivative at one point per
  iteration, yielding the better option of two possible choices for
  each x[i] (hence the name `tree12x`).
*/
#include <cmath>        // for std::abs
#include <vector>
#include <algorithm>    // for std::min_element
#include <argparser.hpp>
#include <minih5.hpp>

#include "utils/viostream.hpp"
#include "utils/timer.hpp"

#include "thousand.hpp"

#include "../tree_12x.hpp"


template<typename float_ = float, typename int_ = int>
void
process_file(
    const char *fname,
    const char *group,
    const int max_iter,
    const bool quiet,
    const bool /* dfs */,
    const bool reorder = false,
    const unsigned PRINT_MAX = 10)
{
    std::vector<float_> xt, y, lam, x;
    std::vector<int_> parent;
    {   // Read extra information
        HDF5 io (fname, "r+");
        io.group(group);

        io.readv(y,      "y");
        io.readv(lam,   "lam");
        io.readv(parent, "parent");

        if (io.has("xt")) {
            io.readv(xt, "xt");
        } else if (io.has("x++")) {
            io.readv(xt, "x++");
        }
    }
    const size_t n = parent.size();
    if (y.size() != n)
        throw std::runtime_error(std::string("y.size() = ") +
                                 std::to_string(y.size()) +
                                 " != " + std::to_string(n));
    x.resize(n);

    tree_12x(n,
             parent.data(),
             y.data(),
             lam[0],
             x.data(),
             -1 /* root*/,
             max_iter,
             !quiet,
             reorder);

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
            max_diff = std::max(max_diff, std::abs(double(x[i] - xt[i])));

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
        ap.add_option('r', "reorder", "Relabel nodes in post-order");
        ap.add_option('q', "quiet",   "Suppress timer output");
        ap.parse(&argc, argv);
        if (argc <= 1) {
            fprintf(stderr, "No tree file!\n");
            return 1;
        }
        const char *fname = argv[1];
        const int max_iter = atoi(ap.get_option("max-iter"));
        typedef int   int_;
        const char *group = "/";
        const bool reorder = ap.has_option("reorder");

        printf("reorder  = %s\n", reorder ? "true" : "false");
        printf("max_iter = %d\n", max_iter);
        {
            if (ap.has_option("float64")) {
                printf("float64\n");
                process_file<double, int_>(fname, group,
                                           max_iter,
                                           ap.has_option("quiet"),
                                           ap.has_option("dfs"),
                                           reorder);
            } else {
                printf("float32\n");
                process_file<float, int_>(fname, group,
                                          max_iter,
                                          ap.has_option("quiet"),
                                          ap.has_option("dfs"),
                                          reorder);
            }
        }
    } catch (ArgParser::ArgParserException &ex) {
        fprintf(stderr, "%s\n", ex.what());
        return 1;
    } catch (const std::exception &ex) {
        fprintf(stderr, "EXCEPTION: %s\n", ex.what());
        return 2;
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
        return 3;
    }
    return 0;
}
