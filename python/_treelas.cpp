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

    struct Double
    {
        double *d;

        Double() : d(new double()) { }
        ~Double() { if (d) delete[] d; }
    };

    m.def("_pass_double_pointer",
          [](Double ptr) -> bool
          {
              if (ptr.d) {
                  *ptr.d = 123.4;
                  return true;
              }
              return false;
          },
          "For tessting purposes only!: Receive a double pointer",
          py::arg("ptr").noconvert())
        ;

    reg_line(m);
    reg_tree(m);
    reg_timer(m);
}
