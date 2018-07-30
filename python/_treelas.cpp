#include <cstdint>
#include <algorithm>                // for std::sort
#include <pybind11/pybind11.h>

#include "../cxx/utils/compiler.hpp"
#include "../cxx/utils/timer.hpp"
#include "../cxx/prufer.hpp"
#include "../cxx/root.hpp"
#include "../cxx/postorder.hpp"

#include "py_np.hpp"

namespace py = pybind11;


void reg_line(py::module &m);
void reg_tree(py::module &m);
void reg_biadjacent(py::module &m);
void reg_spanning(py::module &m);


/**
   For testing: create an array with 3 elements
 */
py::array_t<double>
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

    m.def("post_order",
          [](const py::array_i32 &parent,
             int root,
             const bool include_root) -> py::array_i32
          {
              const size_t n = check_1d_len(parent, "parent");
              if (root < 0)
                  root = find_root(n, parent.data());
              int *postord_buf = post_order(n,
                                            parent.data(),
                                            root);
              return py::array_t<int32_t>({include_root ? n : n-1},
                                          {sizeof(int32_t)}, postord_buf);
          },
          R"pbdoc(
              Compute a post order (DFS finish time) starting at root
              on the tree given by parent
          )pbdoc",
          py::arg("parent"),
          py::arg("root") = -1,
          py::arg("include_root") = false);

    reg_tree(m);
    reg_biadjacent(m);
    reg_spanning(m);
}
