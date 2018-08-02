#include <cstdint>
#include <algorithm>                // for std::sort
#include <pybind11/pybind11.h>

#include "../cxx/utils/compiler.hpp"

#include "py_np.hpp"

namespace py = pybind11;


void reg_line(py::module &m);
void reg_order(py::module &m);
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
    reg_order(m);
    reg_tree(m);
    reg_biadjacent(m);
    reg_spanning(m);
}
