#include <string>
#include <pybind11/pybind11.h>

#include "py_np.hpp"
#include "../cxx/biadjacent.hpp"


namespace py = pybind11;

void
reg_biadjacent(py::module &m)
{
    using IndexIter_int = IndexIter<int>;

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
        .def("__getitem__",
             [](const BiAdjacent &b, const int i) -> IndexIter_int
             {
                 return b[i];
             })
        ;

    py::class_<IndexIter_int>(m, "IndexIter_int")
        .def("__repr__",
             [](const IndexIter_int &self) -> std::string
             {
                 std::ostringstream os;
                 os << "IndexIter_int(" << (void*)&self << ")";
                 return os.str();
             })
        .def("__len__",
             [](const IndexIter_int &self) -> size_t
             {
                 return self.size();
             })
        .def("__iter__",
             [](const IndexIter_int &self)
             {
                 return py::make_iterator(self.begin(), self.end());
             })
        ;
}


PYBIND11_MODULE(adjidx, m)
{
    reg_biadjacent(m);
}


// Local Variables:
// compile-command: "make -C ../build adjidx"
// End:
