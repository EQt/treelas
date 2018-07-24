/*
  Read tree instance from HDF5 file, solve it and write the solution
  to the input.
 */
#include <vector>
#include <iostream>

#include "utils/hdf5.hpp"
#include "utils/timer.hpp"
#include "utils/argparser.hpp"
#include "utils/viostream.hpp"      // std::cout << std::vector<..>

#include "tree_dp.hpp"


// #define DEBUG

template<typename float_ = double>
void
process_tree(const char *fname,
             const bool merge_sort,
             const bool output,
             const int repeat = 5,
             const bool verbose = true)
{
    TimerQuiet _ (verbose);

    std::vector<double> y, lams, xt, x;
    std::vector<int> parent;
    int root = -1;
    double lam, mu = float_(1.0);
    HDF5::Dims ydims;
    {   Timer _ ("Loading Tree");
        HDF5 io (fname, "r+");
        y = io.read<double>("y", &ydims);
        lams = io.read<double>("lam");
        parent = io.read<decltype(parent)::value_type>("parent");
        assert(lams.size() >= 1);
        lam = float_(lams[0]);
        root = parent[0];
        if (root == 1) {
            root--;
            for (auto &i : parent)
                i--;
        }
        if (io.has("xt")) {
            xt = io.read<double>("xt");
        }
    }

    Timer::log("%'d nodes\n", y.size());

    {
        {   Timer _ ("resize x");
                x.resize(y.size());
        }
        for (int r = 0; r < repeat; r++) {
            Timer _ ("tree_dp:\n");
            if (merge_sort)
                tree_dp<true>(y.size(), x.data(), y.data(), parent.data(),
                              lam, mu, root);
            else
                tree_dp<false>(y.size(), x.data(), y.data(), parent.data(),
                               lam, mu, root);
            Timer::stopit();
        }
    }
    if (verbose && x.size() <= 20) {
        std::cout << x << std::endl;
    }
    if (output) {
        Timer _ ("store x");
        HDF5 io (fname, "r+");
        io.owrite("x++", x, &ydims);
    }
}


int
main(int argc, char *argv[])
{
    // test_init_queues();
    // test_merge();
    try {
        ArgParser ap (
            "treesolve [file]\n"
            "\n"
            "Compute exact fused lasso solution on a tree graph.\n"
            "Input [file] is assumed in HDF5 format.\n"
        );
        ap.add_option('m', "merge",     "Use merging (instead of std::sort)");
        ap.add_option('O', "no-output", "Do not write output");
        ap.add_option('r', "repeat",    "Repeat execution", "num", "1");
        ap.parse(&argc, argv);
        if (argc <= 1) {
            fprintf(stderr, "No tree file!\n");
            return 1;
        }
        const char *fname = argv[1];
        setlocale(LC_ALL, "");
        const int repeat = std::atoi(ap.get_option("repeat"));
        process_tree(fname,
                     ap.has_option("merge"),
                     !ap.has_option("no-output"),
                     repeat);
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    }
    return 0;
}
