#include <cstdio>
#include <fstream>
#include <string>         // stoi
#include <memory>         // unique_ptr
#include <argparser.hpp>
#include <graphidx/io/bz2istream.hpp>


int
main(int argc, char *argv[])
{

    ArgParser ap (
        "unbz2 <infile> [outfile]\n"
        "\n"
        "Decompress <infile>"
    );
    ap.add_option('r', "only-read", "Don't write (for benchmarking)");
    ap.add_option('b', "buf-size",  "Buffer size [8192]", "INT", "8192");
    ap.parse(&argc, argv);

    if (argc <= 1) {
        fprintf(stderr, "No input file to decompress!\n\nUSAGE:\n");
        ap.print_usage(stderr);
        return 1;
    }

    const char *infn = argv[1];
    const char *outfn =  (argc >= 2) ? argv[2] : "out.txt";
    const int buf_size = atoi(ap.get_option("buf-size"));
    const bool only_read = ap.has_option("only-read");

    BZ2IStream io (infn, buf_size);
    {
        std::fstream out (outfn, std::ios::out | std::ios::binary);
        if (out) {
            std::unique_ptr<char> buf (new char[buf_size]);
            while (io) {
                io.read(buf.get(), buf_size);
                auto nread = io.gcount();
                only_read || out.write(buf.get(), nread);
            }
        } else {
            fprintf(stderr, "Could not open \"%s\" for writing\n", outfn);
            return 2;
        }
    }

    return 0;
}
