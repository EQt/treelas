#include <string>      // stoi
#include <iostream>
#include <argparser.hpp>
#include <minih5.hpp>
#include <graphidx/io/dimacs10.hpp>
#include <graphidx/io/bz2istream.hpp>
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/thousand.hpp>


int
main(int argc, char *argv[])
{
    ArgParser ap (
        "graph2h5 <graph> [outfile]\n"
        "\n"
        "Decompress `graph` (in DIMACS10 format) and store edges as HDF5"
    );
    ap.add_option('b', "buf-size",  "Buffer size [8192]", "INT", "8192");
    ap.parse(&argc, argv);

    if (argc <= 1) {
        fprintf(stderr, "No input graph!\n\nUSAGE:\n");
        ap.print_usage(stderr);
        return 1;
    }

    const char *infn = argv[1];
    const char *outfn =  (argc > 2) ? argv[2] : "out.h5";
    const int buf_size = atoi(ap.get_option("buf-size"));

    set_thousand_sep(std::cout);

    std::cout << infn << " -> " << outfn << std::endl;
    std::vector<int> head, tail;
    {
        Timer _ ("parse");
        BZ2IStream io (infn, buf_size);
        parse_dimacs10_edges(io, head, tail);
    }
    std::cout << " m = " << head.size() << std::endl;
    {
        Timer _ ("write hdf5");
        HDF5 io (outfn, "w");
        io.owrite("head", head);
        io.owrite("tail", tail);
    }

    return 0;
}