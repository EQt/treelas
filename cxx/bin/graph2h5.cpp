#include <string>      // stoi
#include <iostream>
#include <minih5.hpp>
#include <graphidx/io/dimacs10.hpp>
#include <graphidx/io/bz2istream.hpp>
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/thousand.hpp>


int
main(int argc, char *argv[])
{
    const char *infn = "belgium.bz2";
    const char *outfn = "out.h5";
    if (argc > 1)
        infn = argv[1];
    if (argc > 2)
        outfn = argv[2];
    const size_t buf_size = argc > 3 ? std::stoi(argv[3]) : 8192;
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
