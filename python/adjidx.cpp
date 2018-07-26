#include <string>
#include <pybind11/pybind11.h>

#include "py_np.hpp"
#include "../cxx/biadjacent.hpp"


namespace py = pybind11;


PYBIND11_MODULE(adjidx, m)
{
    py::class_<BiAdjacent>(m, "BiAdjecent")
        .def(py::init([](const py::array_i32 &head,
                         const py::array_i32 &tail)
                      {
                          const auto m = check_1d_len(head);
                          check_len(m, tail, "tail");
                          return BiAdjacent(int(m), head.data(), tail.data());
                      }),
             py::arg("head"),
             py::arg("tail"))
        .def("__repr__",
             [](const BiAdjacent &b) -> std::string
             {
                 return std::string("BiAdjacent[") +
                     "m = " + std::to_string(b.num_edges()) + ", " +
                     "n = " + std::to_string(b.num_nodes()) + "]";
             })
        ;
}


// Local Variables:
// compile-command: "make -C ../build adjidx"
// End:
