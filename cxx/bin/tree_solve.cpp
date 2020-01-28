/*
  Read tree instance from HDF5 file, solve it and write the solution
  to the input.
 */
#include <algorithm>
#include <iostream>
#include <vector>
#include <minih5.hpp>
#include <argparser.hpp>

#include <graphidx/bits/weights.hpp>
#include <graphidx/tree/root.hpp>
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/viostream.hpp>      // std::cout << std::vector<..>


#include "../tree_dp.hpp"


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
    double lam;
    Ones<float_> mu;
    HDF5::Dims ydims;
    {   Timer _ ("Loading Tree");
        HDF5 io (fname, "r+");
        y = io.read<double>("y", &ydims);
        lams = io.read<double>("lam");
        parent = io.read<decltype(parent)::value_type>("parent");
        assert(lams.size() >= 1);
        lam = float_(lams[0]);
        if (io.has("xt")) {
            xt = io.read<double>("xt");
        }
    }
    {
        Timer _ ("min parent\n");
        verbose && printf("min(parent) = %d\n",
                          *std::min_element(parent.begin(), parent.end()));

    }
    int root = -1;
    {   Timer _ ("find root");
        root = find_root(parent);
    }

    Timer::log("%'d nodes\n", y.size());

    {
        {   Timer _ ("resize x");
                x.resize(y.size());
        }
        for (int r = 0; r < repeat; r++) {
            Timer _ ("tree_dp:\n");
            Const<double> clam (lam);
            constexpr bool lazy_sort = true;
            constexpr bool check = false;
            if (merge_sort)
                tree_dp<true, lazy_sort, check>(
                    y.size(),
                    x.data(),
                    y.data(),
                    parent.data(),
                    clam,
                    mu,
                    root
                );
            else
                tree_dp<false, lazy_sort, check>(
                    y.size(),
                    x.data(),
                    y.data(),
                    parent.data(),
                    clam,
                    mu,
                    root
                );
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
    } catch (std::exception &e) {
        fprintf(stderr, "EXCEPTION: %s\n", e.what());
    }
    return 0;
}
