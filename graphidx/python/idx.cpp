#include <string>
#include <stdexcept>
#include <pybind11/pybind11.h>

#include "py_np.hpp"

#include "idx/biadjacent.hpp"
#include "idx/cluster.hpp"
#include "idx/children.hpp"
#include "idx/partition.hpp"
#include "tree/root.hpp"


namespace py = pybind11;


void
reg_idx(py::module &m)
{
    using IndexIter_int = IndexIter<int>;
    using AdjacencyIndex_int = AdjacencyIndex<int>;
    using PartitionIndex_int = PartitionIndex<int>;

    py::class_<AdjacencyIndex_int> PyAdjacencyIndex_int (m, "AdjacencyIndex_int");
    PyAdjacencyIndex_int
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
        .def("num_nodes",
             [](const BiAdjacent &b) -> std::size_t
             {
                 return b.num_nodes();
             })
        .def("num_edges",
             [](const BiAdjacent &b) -> std::size_t
             {
                 return b.num_edges();
             })
        .def("degrees",
             [](const BiAdjacent &b) -> py::array_t<int>
             {
                 const auto n = b.num_nodes();
                 py::array_t<int> deg_ (n);
                 int *deg = deg_.mutable_data();
                 for (size_t i = 0; i < n; i++) {
                     deg[i] = b.index[i+1] - b.index[i];
                 }
                 return deg_;
             })
        ;

    py::class_<ChildrenIndex> (m, "ChildrenIndex", PyAdjacencyIndex_int)
        .def(py::init([](const py::array_i32 &parent,
                         const int root_)
                      {
                          const auto n = check_1d_len(parent);
                          const int root = root_ < 0 ? find_root(n, parent.data()) :
                              root_;
                          if (root < 0)
                              throw std::runtime_error("root not found");
                          return ChildrenIndex(n, parent.data(), root);
                      }),
             py::arg("parent"),
             py::arg("root") = -1)
        .def("__repr__",
             [](const ChildrenIndex &cidx) -> std::string
             {
                 return std::string("ChildrenIndex[ n = ") +
                     std::to_string(cidx.size()) + " ]";
             })
        .def_property_readonly("root", &ChildrenIndex::root_node)
        ;

    py::class_<PartitionIndex_int> (m, "PartitionIndex", PyAdjacencyIndex_int)
        .def(py::init([](const py::array_i32 &ungrouped) {
                          const size_t n = check_1d_len(ungrouped);
                          return PartitionIndex_int(n, ungrouped.data());
                      }),
            py::arg("ungrouped"))
        .def("__repr__",
             [](const PartitionIndex_int &self) -> std::string
             {
                 return std::string("PartionIndex[ n = ") +
                     std::to_string(self.size()) + " ]";
             })
        .def("__iter__",
             [](const PartitionIndex_int &self) {
                 return py::make_iterator(self.begin(), self.end());
             })
        ;

    py::class_<IndexIter_int>(m, "IndexIter_int")
        .def("__repr__",
             [](const IndexIter_int &self) -> std::string
             {
                 std::ostringstream os;
                 os << "<IndexIter_int(" << self.size()
                    <<") @" << (void*)&self << ">";
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

    m.def("cluster",
          [](const py::array_f64 &x,
             const BiAdjacent &neighidx,
             const double eps,
             const int seed) -> PartitionIndex_int {
              return cluster<int, BiAdjacent>(
                  size_t(x.size()),
                  x.data(),
                  neighidx,
                  eps,
                  seed);
          },
          R"pbdoc(
            Find a partition of x such that TO BE CONTINUED
          )pbdoc",
          py::arg("x"),
          py::arg("neighidx"),
          py::arg("eps") = 1e-5,
          py::arg("seed") = 42)
        ;
}


#ifdef OWN_MODULE
PYBIND11_MODULE(idx, m)
{
    reg_idx(m);
}
#endif

// Local Variables:
// compile-command: "make -C ../build pysetup"
// End:
