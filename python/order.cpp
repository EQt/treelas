#include <pybind11/pybind11.h>
#include "py_np.hpp"

#include "tree/bfs.hpp"
#include "tree/prufer.hpp"
#include "tree/postorder.hpp"
#include "tree/root.hpp"

#include "../cxx/utils/timer.hpp"


namespace py = pybind11;


void
reg_order(py::module &m)
{
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
             const int root_,
             const bool include_root,
             const bool verbose) -> py::array_i32
          {
              TimerQuiet _ (verbose);
              const size_t n = check_1d_len(parent, "parent");
              const int root = root_ < 0 ? find_root(n, parent.data()) : root_;
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
          py::arg("include_root") = false,
          py::arg("verbose") = false);
}
