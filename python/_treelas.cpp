#include <cstdint>
#include <algorithm>                // for std::sort
#include <pybind11/pybind11.h>
#include "py_np.hpp"

#include "../extern/glmgen/tf.hpp"
#include "../extern/condat/condat_tv_v2.hpp"
#include "../cxx/utils/compiler.hpp"
#include "../cxx/utils/timer.hpp"
// #include "../cxx/dp_tree.hpp"
#include "../cxx/line.hpp"
#include "../cxx/prufer.hpp"

namespace py = pybind11;


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

    m.def("_empty_array_f64", []() { return py::array_t<double>(); }, R"pbdoc(
        Create an empty np.float64 array
    )pbdoc");

    m.def("_empty_array_i32", []() { return py::array_t<int32_t>(); }, R"pbdoc(
        Create an empty np.int32 array
    )pbdoc");

    m.def("_is_empty", &is_empty, R"pbdoc(
        Tell whether an np.ndarray is empty
    )pbdoc");

    m.def("_stdcxx_sort", [](py::array_f64 a) -> void {
        std::sort(a.mutable_data(), a.mutable_data()+a.size());
      },
      R"pbdoc(
        std::sort from libstdc++ (to be compared against numpy.sort)
      )pbdoc");

    m.def("line_condat", [](const py::array_f64 &y,
                            const double lam,
                            py::array_f64 out)
          -> py::array_t<double> {
              const auto n = check_1d_len(y, "y");
              if (is_empty(out))
                  out = py::array_t<double>({{n}}, {{sizeof(double)}});
              check_len(n, out, "out");
              TV1D_denoise_v2(y.data(),
                              out.mutable_data(),
                              (unsigned int)n,
                              lam);
              return out;
          },
          R"pbdoc(
            Line solver, implemented by Laurent Condat,
            version 2.0, Aug. 30, 2017.
            See: https://www.gipsa-lab.grenoble-inp.fr/~laurent.condat
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none());

    m.def("line_glmgen", [](const py::array_f64 &y,
                            const double lam,
                            py::array_f64 out) -> py::array_t<double>
          {
          #ifdef HAVE_GLMGEN
              const auto n = check_1d_len(y, "y");
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");
              glmgen::tf_dp(int(n),
                            y.data(),
                            lam,
                            out.mutable_data());
              return out;
          #else
              PyErr_SetString(PyExc_NotImplementedError,
                              "glmgen/tf_dp.c was not available");
              return y;
          #endif
          },
          R"pbdoc(
            Line solver (implementation of the R package `glmgen`).
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none());

        m.def("line_las", [](const py::array_f64 &y,
                             const double lam,
                             py::array_f64 out) -> py::array_t<double>
          {
              const size_t n = check_1d_len(y, "y");
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");
              dp_line(n,
                      out.mutable_data(),
                      y.data(),
                      lam);
              return out;
          },
          R"pbdoc(
            Line solver (implementation of the R package `glmgen`).
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none());

    m.def("prufer2parent",
          [](const py::array_i32 &prufer, py::array_i32 parent) -> py::tuple
          {
            const int32_t n = int(check_1d_len(prufer, "prufer") + 2);
            if (is_empty(parent))
                parent = py::array_t<int32_t>({n}, {sizeof(int32_t)});
            else
                check_len(n, parent, "parent");
            const auto r = prufer2parent(n,
                                         prufer.data(),
                                         parent.mutable_data());
            return py::make_tuple(parent, r);
          },
          R"pbdoc(
            Compute parent vector from Prüfer sequence
          )pbdoc",
          py::arg("prufer"),
          py::arg("parent") = py::array_i32());

    /*
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
    py::def("post_order", np_post_ord, (
                arg("parent"),
                arg("root") = 0,
                arg("include_root") = false),
            "Compute the DFS post order starting at root "
            "on the tree given by parent");
    py::register_exceptions();
    */
}
