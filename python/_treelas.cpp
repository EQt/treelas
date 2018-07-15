#include <vector>
#include <cstdint>
#include <stdexcept>
#include <pybind11/pybind11.h>
// #include <pybind11/stl.h>
// #include <pybind11/numpy.h>

#include "../extern/glmgen/tf.hpp"
#include "../extern/condat/condat_tv_v2.hpp"
#include "../cxx/utils/timer.hpp"
// #include "../cxx/dp_tree.hpp"
// #include "../cxx/dp_line.hpp"
// #include "../cxx/prufer.hpp"
#include "../cxx/utils/compiler.hpp"
#include "py_np.hpp"

namespace py = pybind11;

/*
np::ndarray
np_glm_line(const np::ndarray &y,
            const double lam,
            np::ndarray &x,
            bool verbose = false)
{
#ifdef HAVE_GLMGEN
    TimerQuiet _ (verbose);
    const int n = check_1d_len(y, "y");
    if (is_empty(x)) {
        np::dtype dtype = np::dtype::get_builtin<double>();
        py::tuple shape = py::make_tuple(n);
        x = np::empty(shape, dtype);
    }
    {
        Timer("tf_dp");
        glmgen::tf_dp(n,
                      check_data_mut<double>(y, "y"),   // y not const!
                      lam,
                      check_data_mut(x, "x"));
    }
    return x;
#else
    PyErr_SetString(PyExc_NotImplementedError,
                    "glmgen/tf_dp.c was not available");
    return y;
#endif
}
*/


py::array_t<double>
line_condat(const py::array_f64 &y,
            const double lam,
            py::array_f64 out = empty_array<double>())
{
    const auto n = check_1d_len(y, "y");
    if (is_empty(out))
        out = py::array_t<double>({{n}}, {{sizeof(double)}});
    TV1D_denoise_v2(y.data(),
                    out.mutable_data(),
                    (unsigned int)n,
                    lam);
    return out;
}


PYBIND11_MODULE(_treelas, m)
{
    m.doc() = R"pbdoc(
        Treelas C++ Implementation
        --------------------------

        https://github.com/eqt/treelas

        .. currentmodule:: _treelas

        .. autosummary::
           :toctree: _generate

    )pbdoc";
    m.attr("__author__") = "Elias Kuthe <elias.kuthe@tu-dortmund.de>";
    m.attr("__compiler__") = compiler_info();

    m.def("_test_create_array", &_test_create_array, R"pbdoc(
        Test to create an array with elements [13., -1., 42]
    )pbdoc");

    m.def("_empty_array_f64", &empty_array<double>, R"pbdoc(
        Create an empty np.float64 array
    )pbdoc");

    m.def("_empty_array_i32", &empty_array<int32_t>, R"pbdoc(
        Create an empty np.int32 array
    )pbdoc");

    m.def("_is_empty", &is_empty, R"pbdoc(
        Tell whether an np.ndarray is empty
    )pbdoc");

    m.def("line_condat", &line_condat, R"pbdoc(
            Line solver, implemented by Laurent Condat,
            version 2.0, Aug. 30, 2017.
            See: https://www.gipsa-lab.grenoble-inp.fr/~laurent.condat
        )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = empty_array<double>());


    /*
    py::def("prufer2parent", np_prufer2parent, (
                arg("prufer"),
                arg("parent") = empty_array<double>()),
            "Compute parent vector out of prufer sequence");
    py::def("dp_tree", np_dp_tree, (
                arg("y"),
                arg("parent"),
                arg("lam"),
                arg("mu") = 1.0,
                arg("root") = 0,
                arg("x") = empty_array<double>(),
                arg("verbose") = false,
                arg("merge_sort") = false),
            "Dynamic programming algorithm for trees (uniform weighting)");
    py::def("dp_tree_w", np_dp_tree_w, (
                arg("y"),
                arg("parent"),
                arg("lam"),
                arg("mu"),
                arg("root") = 0,
                arg("verbose") = false),
            "Dynamic programming algorithm for trees (node and edge weighting)");
    py::def("dp_dual", np_dp_dual, (
                arg("parent"),
                arg("x"),
                arg("root") = 0,
                arg("alpha") = empty_array<double>()),
            "Compute dual solution along tree");
    py::def("dp_gamma", np_dp_gamma, (
                arg("x"),
                arg("alpha"),
                arg("lam"),
                arg("parent"),
                arg("root_val") = 0.0,
                arg("gamma") = empty_array<double>()),
            "Compute the duality gap vector");
    py::def("dp_forward", np_dp_forward, (
                arg("y"),
                arg("mu"),
                arg("lam")),
            "Forward clipping along a line");
    py::def("dp_reverse", np_dp_reverse, (
                arg("y"),
                arg("mu"),
                arg("lam")),
            "Reverse clipping along a line");
    py::def("dp_line_w", np_dp_line_w, (
                arg("y"),
                arg("mu"),
                arg("lam"),
                arg("x") = empty_array<double>()),
            "Johnson's dynamic programming algorithm");
    py::def("glmgen_line", np_glm_line, (
                arg("y"),
                arg("lam"),
                arg("x") = empty_array<double>(),
                arg("verbose") = false),
            "glmgen implementation of Nick Johnson's O(n) line algorithm");
    py::def("post_order", np_post_ord, (
                arg("parent"),
                arg("root") = 0,
                arg("include_root") = false),
            "Compute the DFS post order starting at root "
            "on the tree given by parent");
    py::register_exceptions();
    */
}
