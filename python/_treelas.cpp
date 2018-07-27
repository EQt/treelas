#include <cstdint>
#include <algorithm>                // for std::sort
#include <pybind11/pybind11.h>

#include "../cxx/utils/compiler.hpp"
#include "../cxx/utils/timer.hpp"

#include "../cxx/prufer.hpp"
#include "../cxx/root.hpp"
#include "../cxx/tree_12.hpp"
#include "../cxx/tree_dp.hpp"
#include "../cxx/tree_dual.hpp"

#include "py_np.hpp"

namespace py = pybind11;

void reg_line(py::module &m);


/**
   For testing: create an array with 3 elements
 */
inline py::array_t<double>
_test_create_array()
{
    double *x = new double[3];
    x[0] = 13.0;
    x[1] = -1.0;
    x[2] = 42.0;
    return py::array_t<double>({3}, {sizeof(double)}, x);
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

    reg_line(m);

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

    m.def("find_root",
          [](const py::array_i32 &parent) -> int32_t
          {
              const size_t n = check_1d_len(parent, "parent");
              return find_root(n, parent.data());
          },
          R"pbdoc(
            Find smallest i such that parent[i] == i
          )pbdoc",
          py::arg("parent"));

    m.def("tree_12",
          [](const py::array_i32 &parent,
             const py::array_f64 &y,
             const double lam,
             int max_iter,
             bool verbose,
             py::array_f64 x) -> py::array_f64
          {
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(parent, "parent");
              check_len(n, y, "y");
              if (is_empty(x))
                  x = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, x, "x");
              approx::tree_12(n, y.data(), lam, parent.data(), x.mutable_data(), max_iter);
              return x;
          },
          R"pbdoc(
            Perform `max_iter` iterations in O(n) time to approximate flsa on tree.
          )pbdoc",
          py::arg("parent"),
          py::arg("y"),
          py::arg("lam"),
          py::arg("max_iter") = 10,
          py::arg("verbose") = false,
          py::arg("x") = py::none());

    m.def("tree_dp",
          [](const py::array_f64 &y,
             const py::array_i32 &parent,
             const double lam,
             const int32_t root,
             py::array_f64 &x,
             const bool verbose,
             const bool merge_sort) -> py::array_f64
          {
              TimerQuiet _ (verbose);
              const double mu = 1.0;
              const auto n = check_1d_len(y, "y");
              check_len(n, parent, "parent");
              if (is_empty(x)) {
                  Timer _ ("allocate x");
                  x = py::array_t<double>({n}, {sizeof(double)});
              }
              check_len(n, x, "x");
              if (merge_sort)
                  tree_dp<false>(n,
                                 x.mutable_data(),
                                 y.data(),
                                 parent.data(),
                                 lam,
                                 mu,
                                 root);
              else
                  tree_dp<false>(n,
                                 x.mutable_data(),
                                 y.data(),
                                 parent.data(),
                                 lam,
                                 mu,
                                 root);
              Timer::stopit();
              return x;
          },
          R"pbdoc(
              Dynamic programming algorithm for trees (uniform weighting)
            )pbdoc",
          py::arg("y"),
          py::arg("parent"),
          py::arg("lam"),
          py::arg("root") = 0,
          py::arg("x") = py::none(),
          py::arg("verbose") = false,
          py::arg("merge_sort") = false);

    m.def("tree_dual",
          [](const py::array_i32 &parent,
             py::array_f64 &x,
             const int root,
             py::array_f64 &alpha) -> py::array_f64
          {
              const auto n = check_1d_len(parent, "parent");
              check_len(n, x, "x");
              if (is_empty(alpha))
                  alpha = py::array_f64({n}, {sizeof(double)});
              check_len(n, alpha, "alpha");
              const int *post_ord = nullptr;
              tree_dual(int(n),
                        x.mutable_data(),
                        parent.data(),
                        post_ord,
                        alpha.mutable_data(),
                        root);
              return alpha;
          },
          R"pbdoc(
              Compute dual solution along tree
          )pbdoc",
          py::arg("parent"),
          py::arg("x"),
          py::arg("root") = 0,
          py::arg("alpha") = py::none());

    m.def("tree_dp_w",
          [](const py::array_f64 &y,
             const py::array_i32 &parent,
             const py::array_f64 &lam,
             const py::array_f64 &mu,
             int root,
             bool verbose,
             py::array_f64 &x) -> py::array_f64
          {
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(y, "y");
              check_len(n, parent, "parent");
              check_len(n, lam, "lam");
              check_len(n, mu, "mu");
              if (is_empty(x))
                  x = py::array_f64({n}, {sizeof(double)});
              check_len(n, x, "x");
              tree_dp_w(n,
                        x.mutable_data(),
                        y.data(),
                        parent.data(),
                        lam.data(),
                        mu.data(),
                        root);
              Timer::stopit();
              return x;
          },
          R"pbdoc(
              Dynamic programming algorithm for trees (node and edge weighting)
          )pbdoc",
          py::arg("y"),
          py::arg("parent"),
          py::arg("lam"),
          py::arg("mu"),
          py::arg("root") = 0,
          py::arg("verbose") = false,
          py::arg("x") = py::none());

    m.def("tree_dual_gap",
          [](const py::array_f64 &x,
             const py::array_f64 &alpha,
             const py::array_f64 &lam,
             const py::array_i32 &parent,
             double root_val,
             py::array_f64 gamma)
          {
              const auto n = check_1d_len(x);
              check_len(n, alpha, "alpha");
              check_len(n, lam, "lam");
              check_len(n, parent, "parent");
              if (is_empty(gamma))
                  gamma = py::array_f64({n}, {sizeof(double)});
              check_len(n, gamma, "gamma");
              tree_dual_gap(n,
                            gamma.mutable_data(),
                            x.data(),
                            alpha.data(),
                            lam.data(),
                            parent.data(),
                            root_val);
              return gamma;
          },
          R"pbdoc(
              Compute the duality gap vector
          )pbdoc",
          py::arg("x"),
          py::arg("alpha"),
          py::arg("lam"),
          py::arg("parent"),
          py::arg("root_val") = 0.0,
          py::arg("gamma") = py::none());

    /*
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
    py::def("post_order", np_post_ord, (
                arg("parent"),
                arg("root") = 0,
                arg("include_root") = false),
            "Compute the DFS post order starting at root "
            "on the tree given by parent");
    */
}
