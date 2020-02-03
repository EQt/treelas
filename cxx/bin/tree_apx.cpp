/*
  Discrete approximation of FLSA on tree graphs.

  This implementation evaluates the derivative at one point per
  iteration, yielding the better option of two possible choices for
  each x[i]
*/
#include <clocale>
#include <cmath>        // for std::abs
#include <vector>
#include <algorithm>    // for std::min_element
#include <argparser.hpp>
#include <minih5.hpp>

#include <graphidx/lina.hpp>
#include <graphidx/utils/viostream.hpp>
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/thousand.hpp>

#include "../tree_apx.hpp"


template<typename float_ = float, typename int_ = int>
void
process_file(
    const char *fname,
    const char *group,
    const int  max_iter,
    const bool quiet,
    const bool /* dfs */,
    const bool reorder,
    const double lam_override,
    const unsigned PRINT_MAX = 10)
{
    std::vector<float_> xt, y, x;
    std::vector<int_> parent;
    double lam;
    {   Timer _ ("load hdf5");
        HDF5 io (fname, "r");
        io.group(group);

        io.readv(y,      "y");
        auto lams = io.read<double>("lam");
        assert(lams.size() >= 1);
        lam = std::isnan(lam_override) ? float_(lams[0]) : lam_override;
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
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "lam = " << lam << std::endl;
        std::cout << "n = " << y.size() << std::endl;
        std::cout << std::endl;
    }
    tree_apx(n,
             parent.data(),
             y.data(),
             float_(lam),
             x.data(),
             -1 /* root*/,
             max_iter,
             !quiet,
             reorder);

    if (n <= PRINT_MAX) {
        fprintf(stdout, " x: ");
        print_double_list(x, 5, stdout);
        if (xt.size() == n) {
            fprintf(stdout, "xt: ");
            print_double_list(xt, 5, stdout);
        }
    }

    if (xt.size() == n)
        fprintf(stderr, "Norm(x - xt, Inf):  %g\n", max_abs_diff(x, xt));
}


int
main(int argc, char *argv[])
{
    try {
        set_thousand_sep(std::cout, '_');
        setlocale(LC_ALL, "C");

        ArgParser ap ("tree_apx [file]\n");
        ap.add_option('i', "max-iter",
                      "Number of iterations [default 3]",
                      "INT", "3");
        ap.add_option('d', "dfs",     "Use DFS instead of BFS order");
        ap.add_option('6', "float64", "Calculate in float64_t precision");
        ap.add_option('r', "reorder", "Relabel nodes in post-order");
        ap.add_option('q', "quiet",   "Suppress timer output");
        ap.add_option('l', "lam",     "Tuning parameter λ", "num", "nan");
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

        printf("%s\n", fname);
        printf("reorder  = %s\n", reorder ? "true" : "false");
        printf("max_iter = %d\n", max_iter);
        {
            if (ap.has_option("float 64")) {
                printf("float64\n");
                process_file<double, int_>(fname, group,
                                           max_iter,
                                           ap.has_option("quiet"),
                                           ap.has_option("dfs"),
                                           reorder,
                                           std::atof(ap.get_option("lam")));
            } else {
                printf("float32\n");
                process_file<float, int_>(fname, group,
                                          max_iter,
                                          ap.has_option("quiet"),
                                          ap.has_option("dfs"),
                                          reorder,
                                          std::atof(ap.get_option("lam")));
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
