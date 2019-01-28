/*
  Discrete approximation of FLSA on tree graphs.

  This implementation evaluates the derivative at one point per
  iteration, yielding the better option of two possible choices for
  each x[i] (hence the name `tree12`).
*/
#include <cmath>        // for std::abs
#include <vector>
#include <algorithm>    // for std::min_element
#include <argparser.hpp>
#include <minih5.hpp>

#include "utils/timer.hpp"

#include "thousand.hpp"

#include "../tree_io.hpp"
#include "../tree_12.hpp"


template<typename float_, typename int_>
void
compute_orders(TreeLasso<float_, int_> &tree,
               std::vector<int_> &ipostord,
               std::vector<int_> &iorder,
               const bool use_dfs,
               const size_t PRINT_MAX = 0)
{
    const auto n = tree.parent.size();
    {   Timer _ ("Computing BFS");
        if (use_dfs) {
            tree.preorder = std::move(tree.dfs);
        } else {
            tree.preorder = (tree.bfs.size() == n) ?
                tree.bfs : compute_bfs(tree.parent, tree.root);
        }
        // for testing:
        // preorder = {0, 2, 3, 4, 1, 5, 6, 7};
        tree.postorder = reverse(tree.preorder);
        ipostord = iperm(tree.postorder);
        iorder = concat(ipostord, tree.preorder);
    }
    if (n <= PRINT_MAX) {
        printf("   parent: ");
        print(tree.parent, 0);
        printf(" preorder: ");
        print(tree.preorder, 0);
        printf("postorder: ");
        print(tree.postorder, 0);
        printf(" ipostord: ");
        print(ipostord, 0);
    }
}


template<typename float_, typename int_>
std::vector<double>
tree_12(const TreeLasso<float_, int_> &tree,
        const std::vector<int_> &ipostordv,
        const std::vector<int_> &iorderv,
        const size_t max_iter = 20)
{
    const auto n = tree.parent.size();
    std::vector<double> xv (n);

    const float_ lam = tree.lam[0];
    const float_ *y = tree.y.data();
    const int_ *parent = tree.parent.data();
    const int  *order = tree.postorder.data();
    const int_ *ipostord = ipostordv.data();
    const int_ *iorder = iorderv.data();
    double *x = xv.data();

    using approx::tree_12;

    tree_12(n, y, lam, parent, order, iorder, ipostord, x, max_iter);

    return xv;
}



template<typename float_ = float, typename int_ = int>
void
process_tree(const char *fname,
             const char *group,
             const int max_iter,
             const bool use_dfs,
             const unsigned PRINT_MAX = 10)
{
    using namespace approx;
    auto tree = load_treelasso<float_, int_>(fname, group);
    const auto n = tree.parent.size();
    std::cout << "n = " << n << std::endl;
    std::vector<int_> ipostord, iorder;
    compute_orders(tree, ipostord, iorder, use_dfs, PRINT_MAX);
    const auto x = tree_12(tree, ipostord, iorder, max_iter);

    std::vector<double> xt;
    {   // Read extra information
        HDF5 io (fname, "r+");
        if (io.has("xt")) {
            xt = io.read<double>("xt");
        } else if (io.has("x++")) {
            xt = io.read<double>("x++");
        }
    }
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

        ArgParser ap ("tree_apx [file]\n");
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
            printf("sizeof(struct Node): %d bytes\n",
                   int(sizeof(approx::Node<double, int_>)));
            process_tree<double, int_>(fname, group,
                                       max_iter,
                                       ap.has_option("dfs"));
        } else {
            printf("sizeof(struct Node): %d bytes\n",
                   int(sizeof(approx::Node<float, int_>)));
            process_tree<float, int_>(fname, group,
                                       max_iter,
                                       ap.has_option("dfs"));
        }
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    } catch (std::exception &e) {
        fprintf(stderr, "EXCEPTION: %s\n", e.what());
    }
    return 0;
}
