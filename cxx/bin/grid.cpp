#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <argparser.hpp>
#include <minih5.hpp>

#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/thousand.hpp>


template <typename int_ = int>
void
iter_grid_edges_pixel(const int_ n1,
                      const int_ n2,
                      std::function<void(int_, int_, int_, int_)> proc)
{
    constexpr int_ dx = int_(1), dy = int_(0);
    for (int_ i = 0; i < n1 - dx; i++)
        for (int_ j = 0; j < n2 - dy; j++)
            proc(i, j, i+dx, j+dy);
    for (int_ i = dy; i < n1; i++)
        for (int_ j = 0; j < n2 - dx; j++)
            proc(i, j, i-dy, j+dx);
}


template <typename int_ = int>
void
iter_grid_edges(const int_ n1,
                const int_ n2,
                std::function<void(int_, int_)> proc)
{
    iter_grid_edges_pixel<int_>(n1, n2,
                                [=](int_ x1, int_ y1, int_ x2, int_ y2) {
                                    proc(x1 + y1*n1, x2 + y2*n1);
                                });
}


template <typename int_ = int>
void
generate(const int n1, const int n2, const char *outfn)
{
    if (n1 <= 0)
        throw std::runtime_error(std::string("n1 = ") + std::to_string(n1));
    if (n2 <= 0)
        throw std::runtime_error(std::string("n2 = ") + std::to_string(n2));

    std::vector<int_> head, tail;
    head.reserve(n1*n2);
    tail.reserve(n1*n2);

    {
        Timer _ ("generate edges");
        iter_grid_edges<int_>(n1, n2,
                              [&](int_ u, int_ v) {
                                  head.push_back(u);
                                  tail.push_back(v);
                              });
    }
    {
        Timer _ ("hdf5 output");
        HDF5 io (outfn, "w");
        io.owrite("head", head);
        io.owrite("tail", tail);
    }
}


int
main(int argc, char *argv[])
{
    try {
        ArgParser ap (
            "grid\n"
            "\n"
            "Generate edges of grid graph and store them as HDF5 file"
        );
        ap.add_option('o', "out", "Output file", "FILE", "grid.h5");
        ap.add_option('i', "img", "Image (HDF5) => sizes", "FILE");
        ap.add_option('w', "width", "Image (HDF5) => sizes", "INT", "5");
        ap.add_option('h', "height", "Image (HDF5) => sizes", "INT", "3");
        ap.parse(&argc, argv);

        int n1 = -1, n2 = -2;
        if (ap.has_option("img")) {
            const auto fname = ap.get_option("img");
            HDF5 in5 (fname, "r");
            if (!in5.has("y"))
                throw std::runtime_error(std::string("y needed in ") + fname);
            auto dims = in5.dimensions("y");
            if (dims.size() != 2)
                throw std::runtime_error(
                    std::string("y needs two dimensions, got ") +
                    std::to_string(dims.size()));
            n1 = decltype(n1)(dims[0]);
            n2 = decltype(n2)(dims[1]);
        } else {
            n1 = atoi(ap.get_option("height"));
            n2 = atoi(ap.get_option("width"));
        }
        const char *outfn = ap.get_option("out");
        set_thousand_sep(std::cout);

        std::cout << n1 << "x" << n2 << " => " << outfn << std::endl;
        generate(n1, n2, outfn);
    } catch (std::runtime_error &e) {
        fprintf(stderr, "EXCEPTION: %s\n", e.what());
        return 1;
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
        return 2;
    }

    return 0;
}


// Local Variables:
// compile-command: "cd ../../build/ && make grid && ./grid 3 4"
// End:
