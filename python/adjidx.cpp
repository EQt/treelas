#include <string>
#include <stdexcept>
#include <pybind11/pybind11.h>

#include "../cxx/biadjacent.hpp"
#include "../cxx/children.hpp"
#include "../cxx/root.hpp"

#include "py_np.hpp"

namespace py = pybind11;


void
reg_biadjacent(py::module &m)
{
    using IndexIter_int = IndexIter<int>;
    using AdjacencyIndex_int = AdjacencyIndex<int>;

    py::class_<AdjacencyIndex_int> PyAdjacencyIndex_int (m, "AdjacencyIndex_int");
    PyAdjacencyIndex_int
        // .def(py::init([](const py::array_i32 &value,
        //                  const py::array_i32 &index)
        //               {
        //                   return new AdjacencyIndex_int(
        //                       std::vector<int>(value.data(), value.data() + value.size()),
        //                       std::vector<int>(index.data(), value.data() + value.size())
        //                   );
        //               }),
        //      py::arg("value"),
        //      py::arg("index"))
        .def("__getitem__",
             [](const AdjacencyIndex_int &b, const int i) -> IndexIter_int
             {
                 return b[i];
             })
        .def("__len__",
             [](const AdjacencyIndex_int &self) -> size_t
             {
                 return self.size();
             })

        ;

    py::class_<BiAdjacent> (m, "BiAdjacent", PyAdjacencyIndex_int)
        .def(py::init([](const py::array_i32 &head,
                         const py::array_i32 &tail)
                      {
                          const auto m = check_1d_len(head);
                          check_len(m, tail, "tail");
                          return new BiAdjacent(int(m), head.data(), tail.data());
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

    py::class_<ChildrenIndex> (m, "ChildrenIndex", PyAdjacencyIndex_int)
        .def(py::init([](const py::array_i32 &parent,
                         const int root_)
                      {
                          const auto n = check_1d_len(parent);
                          const int root = root_ < 0 ? find_root(n, parent.data()) : root_;
                          if (root < 0)
                              throw std::runtime_error("root not found");
                          return ChildrenIndex(n, parent.data(), root);
                      }),
             py::arg("parent"),
             py::arg("root") = 0)
        .def("__repr__",
             [](const ChildrenIndex &cidx) -> std::string
             {
                 return std::string("ChildrenIndex[ n = ") +
                     std::to_string(cidx.size()) + "]";
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


#ifdef OWN_MODULE
PYBIND11_MODULE(adjidx, m)
{
    reg_biadjacent(m);
}
#endif

// Local Variables:
// compile-command: "make -C ../build pysetup"
// End:
