#include <fstream>
#include <string>      // stoi
#include <memory>      // unique_ptr
#include <graphidx/io/bz2istream.hpp>


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
