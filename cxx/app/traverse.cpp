/**
   Read a graph.
   If it is not a tree, sample a random spanning tree.
   Traverse the graph and print timings.
*/
#include <cstdint>
#include <iostream>

#include "../utils/argparser.hpp"
#include "../utils/hdf5.hpp"
#include "../utils/timer.hpp"
#include "../utils/thousand.hpp"

#include "idx/biadjacent.hpp"
// #include "../kruskal_mst.hpp"
#include "../spanning/prim_mst.hpp"


void
traverse(const char *fname, const char *group = "/", const int seed = 2018)
{
    std::vector<int> head, tail;
    {   Timer _ ("load hdf5");
        HDF5 io (fname, "r+");
        io.group(group);
        if (io.has("head"))
            head = io.read<decltype(head)::value_type>("head");
        if (io.has("tail"))
            tail = io.read<decltype(tail)::value_type>("tail");
    }

    const size_t m = head.size();
    Timer tim ("compute edgxe index");
    BiAdjacent index (head, tail);
    tim.stop();
    const size_t n = index.num_nodes();

    std::vector<int> parent;
    {   Timer _ ("random span");
        parent = random_spanning_tree(index, seed);
    }

    {   Timer _ ("store parent");
        HDF5 io (fname, "r+");
        io.group(group);
        io.owrite("parent", parent);
    }

    std::cout << "m = " << m << std::endl
              << "n = " << n << std::endl;

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
        set_thousand_sep(std::cout, '\'');
        traverse(fname, ap.get_option("group"), seed);
    } catch (std::runtime_error &e) {
        fprintf(stderr, "EXCEPTION: %s\n", e.what());
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    }

    return 0;
}


// Local Variables:
// compile-command: "cd ../build/ && make traverse && ./traverse ../data/snap/com-youtube.h5"
// End:
