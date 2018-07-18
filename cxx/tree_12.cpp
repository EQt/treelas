/*
  Discrete approximation of FLSA on tree graphs.

  This implementation evaluates the derivative at one point per
  iteration, yielding the better option of two possible choices for
  each x[i] (hence the name `tree12`).
*/
#include <cmath>        // for std::abs
#include <vector>
#include <algorithm>    // for std::min_element
#include <cassert>
#include "treeio.hpp"
#include "tree12.hpp"
#include "utils/hdf5.hpp"
#include "utils/timer.hpp"
#include "utils/argparser.hpp"



template<typename float_ = float, typename int_ = int>
void
process_tree(const char *fname,
             const char *group,
             const int max_iter,
             const bool use_dfs,
             const int PRINT_MAX = 10)
{
    using namespace disc;
    auto tree = load_treelasso<float_, int_>(fname, group);
    const int n = tree.parent.size();

    std::vector<int_> ipostord, iorder;
    compute_orders(tree, ipostord, iorder, use_dfs, PRINT_MAX);
    const auto x = tree12(tree, ipostord, iorder, max_iter);

    std::vector<double> xt;
    {   // Read extra information
        HDF5 io (fname, "r+");
        if (io.has("xt")) {
            xt = io.read<double>("xt");
        }
    }
    if (n <= PRINT_MAX) {
        fprintf(stdout, " x: ");
        print(x, 3, stdout);
        if (int(xt.size()) == n) {
            fprintf(stdout, "xt: ");
            print(xt, 3, stdout);
        }
    }

    if (int(xt.size()) == n) {
        double max_diff = 0.0;
        for (int i = 0; i < n; i++) {
            max_diff = std::max(max_diff, std::abs(x[i] - xt[i]));
        }
        fprintf(stdout, "Norm(x - xt, Inf):  %g\n", max_diff);
    }
}


int
main(int argc, char *argv[])
{
    #if TEST_NODE
    {
        printf("Testing same/parent\n");
        Node n;
        n.set_parent(42);
        n.set_same(true);
        printf("%d\n%d\n", n.parent(), int(n.same()));
        n.set_same(false);
        printf("%d\n%d\n", n.parent(), int(n.same()));
    }
    #endif
            
    try {
        ArgParser ap ("tree12 [file]\n");
        ap.add_option('i', "max-iter",
                      "Number of iterations [default 3]",
                      "INT", "3");
        ap.add_option('d', "dfs", "Use DFS instead of BFS order");
        ap.parse(&argc, argv);
        if (argc <= 1) {
            fprintf(stderr, "No tree file!\n");
            return 1;
        }
        const char *fname = argv[1];
        const int max_iter = atoi(ap.get_option("max-iter"));
        {
            typedef float float_;
            typedef int   int_;
            printf("sizeof(struct Node): %d bytes\n",
                   int(sizeof(disc::Node<float_, int_>)));
            printf("max_iter = %d\n", max_iter);
            const char *group = "/";
            process_tree<float_, int_>(fname, group, max_iter, ap.has_option("dfs"));
        }
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    }
    return 0;
}
