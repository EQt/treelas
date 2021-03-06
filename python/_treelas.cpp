#include <cmath>        // std::nan
#include <cstdint>
#include <chrono>
#include <thread>
#include <pybind11/pybind11.h>

#include <graphidx/utils/compiler.hpp>
#include <graphidx/utils/timer.hpp>
#include <graphidx/utils/perftimer.hpp>
#include "py_np.hpp"

namespace py = pybind11;


void reg_line(py::module &m);
void reg_tree(py::module &m);
void reg_timer(py::module &m);

/**
   For testing: create an array with 3 elements
 */
py::array_t<double>
_test_create_array()
{
    auto arr = py::array_t<double>({3}, {sizeof(double)});
    double *x = arr.mutable_data();
    x[0] = 13.0;
    x[1] = -1.0;
    x[2] = 42.0;
    return arr;
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
    m.attr("__asan__") = asan_enabled();

    m.def("_test_create_array", &_test_create_array,
          py::return_value_policy::move, R"pbdoc(
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

    m.def("_pointer",
          [](const py::array_i32 &arr) -> size_t { return (size_t) arr.data(); });

    m.def("_pointer",
          [](py::object v) {
              if (v) {
                  v = py::float_(42.0);
                  return 1;
              }
              return 0;
          },
          R"pbdoc(
            Experiments on hot to pass by reference.
          )pbdoc",
          py::arg("v").none(true) = py::none()
        );

    m.def("_pass_timer_ref",
          [](Timer *timer = nullptr) -> double
          {
              TimerQuiet _ (true);
              if (timer) {
                  py::print("start\n");
                  timer->start("sleep(50msec)");
                  std::this_thread::sleep_for (std::chrono::milliseconds(50));
                  timer->stop();
                  return double(*timer);
              } else {
                  py::print("no timer");
              }
              return -1.0;
          },
          "For testing purposes only!: Obtain a Timer object as pointer argument",
          py::arg("timer").none(true) = py::none())
        ;

    reg_line(m);
    reg_tree(m);
    reg_timer(m);
}
