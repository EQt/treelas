/**
   Optimize a fused lasso instance on a general graph by iteratively close the "gaps".
*/
#include <cstdio>
#include <argparser.hpp>
#include <minih5.hpp>

#include <graphidx/idx/biadjacent.hpp>
#include <graphidx/bits/weights.hpp>
#include <graphidx/utils/timer.hpp>

#include "../gaplas.hpp"


template <typename int_t = int>
BiAdjacentIndex<int_t>
read_graph(const char *fname, const char *group = "/")
{
    HDF5 io (fname, "r");
    io.group(group);
    const auto head = io.read<int_t>("head");
    const auto tail = io.read<int_t>("tail");
    return BiAdjacentIndex<int_t>(head, tail);
}


void
optimize(
    const char *fname,
    const double lam,
    const char */* out_name */,
    const bool /* overwrite = true */,
    const size_t max_iter,
    const char *group = "/")
{
    std::vector<double> y, x;
    {
	HDF5 io (fname, "r");
	io.group(group);
	y = io.read<decltype(y)::value_type>("y");
    }
    const auto index = read_graph(fname, group);
    Const<double> clam (lam);
    const auto n = index.size();
    if (n != y.size())
	throw std::runtime_error(
	    std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
	    std::to_string(n) + " != " + std::to_string(y.size())
	);
    x.resize(n);
    gaplas(x.data(), y.data(), index, lam, max_iter);
}


int
main(int argc, char* argv[])
{
    try {
        ArgParser ap (
            "gaplas [file] [out_id]\n"
            "\n"
            "Approximate fused lasso by iteratively close the \"gaps\""
        );
        ap.add_option('l', "lam", "Tuning parameter λ", "num", "nan");
	ap.add_option('i', "iter", "Random seed [default 2018]", "INT", "10");
        ap.add_option('g', "group", "HDF5 group [default \"/\"]", "STR", "/");
        ap.add_option('f', "force", "Force to overwrite solution");
	ap.parse(&argc, argv);
        if (argc <= 1) {
            fprintf(stderr, "No file!\n");
            return 1;
        }
	const char *fname = argv[1];
	const char *x_opt = argc > 2 ? argv[2] : "x_gap";
	const double lam = std::atof(ap.get_option("lam"));
	const int max_iter = std::atoi(ap.get_option("iter"));
	optimize(fname, lam, x_opt, ap.has_option("force"), max_iter);
    } catch (std::runtime_error &e) {
        fprintf(stderr, "EXCEPTION: %s\n", e.what());
    } catch (const char *msg) {
        fprintf(stderr, "EXCEPTION: %s\n", msg);
    }

    return 0;
}
