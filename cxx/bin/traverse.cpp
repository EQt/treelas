/**
   Read a graph.
   If it is not a tree, sample a random spanning tree.
   Traverse the graph and print timings.
*/
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <argparser.hpp>
#include <minih5.hpp>

#include <graphidx/utils/timer.hpp>
#include <graphidx/idx/biadjacent.hpp>
#include <graphidx/spanning/connected.hpp>
#include <graphidx/spanning/prim_mst.hpp>
#include <graphidx/utils/thousand.hpp>


void
traverse(
    const char *fname,
    const char *outfn,
    const char *group = "/",
    const int seed = 2018)
{
    std::vector<int> head, tail;
    {   Timer _ ("load hdf5");
        HDF5 io (fname, "r");
        io.group(group);
        head = io.read<decltype(head)::value_type>("head");
        tail = io.read<decltype(tail)::value_type>("tail");
    }

    const size_t m = head.size();
    Timer tim ("compute edgxe index");
    BiAdjacent index (head, tail);
    tim.stop();
    const size_t n = index.num_nodes();
    {
        Timer _ ("min nodes\n");
        std::cout << "  min(head) = "
                  << *std::min_element(head.begin(), head.end())
                  << std::endl;
        std::cout << "  min(tail) = "
                  << *std::min_element(tail.begin(), tail.end())
                  << std::endl;
        std::cout << "  m = " << m << std::endl
                  << "  n = " << n << std::endl;
    }
    {
        Timer _ ("largest cc\n");
        auto largest_cc = connected_components(index).largest().sorted();
        std::cout << " n' = " << largest_cc.size() << std::endl;
        if (largest_cc.size() > n)
            throw std::runtime_error("traverse(): Should not happen");
        if (largest_cc.size() < n) {
            {
                Timer _ ("store cc");
                HDF5 io (fname, "r+");
                io.group(group);
                io.owrite("largest_cc", largest_cc);
            }
            {
                Timer _ ("induced subgraph");
                index.induced_subgraph(largest_cc);
            }
        }
    }
    std::vector<int> parent;
    {   Timer _ ("random span");
        parent = random_spanning_tree(index, seed);
    }
    {
        Timer _ ("min parent:\n");
        const auto min_parent =
            *std::min_element(parent.begin(), parent.end());
        std::cout << "  min(parent) = " << min_parent << std::endl;
        if (min_parent < 0)
            throw std::runtime_error("min(parent) is negative!");
    }

    {
        const char *mode = "w";
        if (outfn == nullptr) {
            outfn = fname;
            mode = "r+";
        }
        Timer _ ("store parent");
        HDF5 io (outfn, mode);
        io.set_compression(3);
        io.group(group);
        io.owrite("parent", parent);
    }
}


int
main(int argc, char *argv[])
{
    try {
        ArgParser ap (
            "traverse [file]\n"
            "\n"
            "Read in a graph (edge list) and compute a random spanning tree.\n"
            "Store this tree (parent)."
        );
        ap.add_option('s', "srand", "Random seed [default 2018]", "INT", "2018");
        ap.add_option('g', "group", "HDF5 group [default \"/\"]", "STR", "/");
        ap.parse(&argc, argv);
        if (argc <= 1) {
            fprintf(stderr, "No file!\n");
            return 1;
        }
        const int seed = atoi(ap.get_option("srand"));
        const char *fname = argv[1];
        const char *outfn = argc > 2 ? argv[2] : fname;
        set_thousand_sep(std::cout);
        std::cout << fname << " => " << outfn << std::endl;
        traverse(fname, outfn, ap.get_option("group"), seed);
    } catch (std::runtime_error &e) {
        fprintf(stderr, "EXCEPTION: %s\n", e.what());
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    }

    return 0;
}


// Local Variables:
// compile-command: "cd ../../build/ && make traverse && ./traverse ../data/snap/com-youtube.h5"
// End:
