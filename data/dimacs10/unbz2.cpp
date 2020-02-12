#include <cstdio>
#include <string>      // stoi
#include <memory>      // unique_ptr
#include "bz2istream.hpp"


int
main(int argc, char *argv[])
{
    const char *infn = "belgium.bz2";
    const char *outfn = "belgium.txt";
    if (argc > 1)
        infn = argv[1];
    if (argc > 2)
        outfn = argv[2];
    const int buf_size = argc > 3 ? std::stoi(argv[3]) : 2*4096;
    const bool only_read = argc > 4;

    auto out = fopen(outfn, "wb");
    if (!out) {
        fprintf(stderr, "Could not open \"%s\" for writing\n", infn);
        return 2;
    }

    BZ2IStream io (infn);

    std::unique_ptr<char> buf (new char[buf_size]);
    while (io) {
        io.read(buf.get(), buf_size);
        auto nread = io.gcount();
        only_read || fwrite(buf.get(), nread, 1, out);
    }
    fclose(out);
    return 0;
}
