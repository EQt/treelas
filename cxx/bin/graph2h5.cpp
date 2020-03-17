#include <string>      // stoi
#include <iostream>
#include <argparser.hpp>
#include <minih5.hpp>
#include <graphidx/io/dimacs10.hpp>
#include <graphidx/io/snap.hpp>
#include <graphidx/io/autoistream.hpp>
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
    ap.add_option('s', "snap",  "Use SNAP format");
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
    try {
	std::vector<int> head, tail;
	if (!ap.has_option("snap")) {
	    Timer _ ("parse dimacs10");
	    AutoIStream io (infn, buf_size);
	    parse_dimacs10_edges(io, head, tail);
	} else {
	    Timer _ ("parse snap");
	    AutoIStream io (infn, buf_size);
	    parse_snap_edges(io, head, tail);
	}
	std::cout << " m = " << head.size() << std::endl;
	{   Timer _ ("write hdf5");
	    HDF5 io (outfn, "w");
	    io.owrite("head", head);
	    io.owrite("tail", tail);
	}
    } catch (std::exception &e) {
	std::cerr << "EXCEPTION: " << e.what() << std::endl;
	return -1;
    } catch (...) {
	std::cerr << "SOME ERROR OCCURRED" << std::endl;
	return -2;
    }

    return 0;
}
