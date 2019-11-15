#include <bzlib.h>
#include <cstdio>
#include <string>      // stoi
#include <memory>


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

    auto in = BZ2_bzopen(infn, "rb");
    if (!in) {
        fprintf(stderr, "Could not open \"%s\"\n", infn);
        return 1;
    }

    auto out = fopen(outfn, "wb");
    if (!out) {
        fprintf(stderr, "Could not open \"%s\" for writing\n", infn);
        BZ2_bzclose(in);
        return 2;
    }

    std::unique_ptr<char> buf (new char[buf_size]);
    while (true) {
        auto nread = BZ2_bzread(in, buf.get(), buf_size);
        if (nread <= 0)
            break;
        only_read || fwrite(buf.get(), buf_size, 1, out);
    }
    int errnum;
    fprintf(stdout, "finally: %s\n",  BZ2_bzerror(in, &errnum));

    fclose(out);
    BZ2_bzclose(in);
    return 0;
}
