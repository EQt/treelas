#include <pybind11/pybind11.h>
#include "py_np.hpp"
#include "weights.hpp"

#include <graphidx/utils/timer.hpp>       // for TimerQuiet

#include "../cxx/tree_apx.hpp"
#include "../cxx/tree_dp.hpp"
#include "../cxx/tree_dual.hpp"


namespace py = pybind11;


void
reg_tree(py::module &m)
{
    m.def("tree_12x",
          [](const py::array_i32 &parent,
             const py::array_f64 &y,
             const double lam,
             int max_iter,
             bool verbose,
             py::array_f64 x) -> py::array_f64
          {
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(parent, "parent");
              const int root = -1;
              check_len(n, y, "y");
              if (is_empty(x))
                  x = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, x, "x");
              tree_12x(n, parent.data(), y.data(), lam, x.mutable_data(), root, max_iter);
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
             const double mu,
             py::array_f64 &x,
             const bool verbose,
             const bool merge_sort,
             const bool lazy_sort) -> py::array_f64
          {
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(y, "y");
              check_len(n, parent, "parent");
              if (is_empty(x)) {
                  Timer _ ("allocate x");
                  x = py::array_t<double>({n}, {sizeof(double)});
              }
              check_len(n, x, "x");
              if (merge_sort) {
                  tree_dp<true>(n,
                                x.mutable_data(),
                                y.data(),
                                parent.data(),
                                lam,
                                mu,
                                root);
              } else {
                  if (lazy_sort)
                      tree_dp<false, true>(n,
                                           x.mutable_data(),
                                           y.data(),
                                           parent.data(),
                                           lam,
                                           mu,
                                           root);
                  else
                      tree_dp<false, false>(n,
                                            x.mutable_data(),
                                            y.data(),
                                            parent.data(),
                                            lam,
                                            mu,
                                            root);
              }
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
          py::arg("mu") = 1.0,
          py::arg("x") = py::none(),
          py::arg("verbose") = false,
          py::arg("merge_sort") = false,
          py::arg("lazy_sort") = false);

    m.def("tree_dual",
          [](const py::array_i32 &parent,
             py::array_f64 &x,
             const int root,
             py::array_f64 &alpha,
             const py::array_i32 &post_ord,
             const bool tree_orientation,
             const bool verbose) -> py::array_f64
          {
              const auto n = check_1d_len(parent, "parent");
              check_len(n, x, "x");
              if (is_empty(alpha))
                  alpha = py::array_f64({n}, {sizeof(double)});
              check_len(n, alpha, "alpha");
              TimerQuiet _ (verbose);
              tree_dual(int(n),
                        x.mutable_data(),
                        parent.data(),
                        is_empty(post_ord) ? nullptr : post_ord.data(),
                        alpha.mutable_data(),
                        root,
                        tree_orientation);
              return alpha;
          },
          R"pbdoc(
              Compute dual solution along tree.
              Be aware that `z` is supposed to be `mu*(x - y)` and will be changed.
              After the computation, `z[root]` will contain `z.sum()`
              (from before the computation).

              If `tree_orientation`, every tree edge is directed from `i` to `parent[i]`;
              otherwise it goes from `min(i, parent[i])` to `max(i, parent[i])`.
          )pbdoc",
          py::arg("parent"),
          py::arg("z"),
          py::arg("root") = 0,
          py::arg("alpha") = py::none(),
          py::arg("post_ord") = py::array_i32(),
          py::arg("tree_orientation") = true,
          py::arg("verbose") = false);

    m.def("tree_dp",
          [](const py::array_f64 &y,
             const py::array_i32 &parent,
             const py::array_f64 &lam,
             const py::array_f64 &mu,
             int root,
             const bool verbose,
             const bool lazy_sort,
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
              {   Timer _ ("check finite");
                  check_all_finite(y.data(),  n, "y");
                  check_all_finite(mu.data(), n, "mu");
                  const size_t i = find_non_finite(lam.data(), n);
                  if (i < n && int(i) != root) {
                      throw std::runtime_error(
                          std::string("lam[") + std::to_string(i) +
                          "] = " + std::to_string(lam.unchecked<1>()(i)) +
                          " ; root = " + std::to_string(root));
                  }
              }

              constexpr auto merge_sort = true;
              constexpr auto check_zero = true;
              if (lazy_sort)
                  tree_dp<merge_sort, true, check_zero>(
                      n,
                      x.mutable_data(),
                      y.data(),
                      parent.data(),
                      convert(lam),
                      convert(mu),
                      root);
              else
                  tree_dp<merge_sort, false, check_zero>(
                      n,
                      x.mutable_data(),
                      y.data(),
                      parent.data(),
                      convert(lam),
                      convert(mu),
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
          py::arg("lazy_sort") = false,
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
              check_len(n, parent, "parent");
              if (is_empty(gamma))
                  gamma = py::array_f64({n}, {sizeof(double)});
              check_len(n, gamma, "gamma");
              if (lam.ndim() == 1) {
                  check_len(n, lam, "lam");
                  tree_dual_gap(n,
                                gamma.mutable_data(),
                                x.data(),
                                alpha.data(),
                                lam.data(),
                                parent.data(),
                                root_val);
              } else if (lam.ndim() == 0) {
                  // throw std::runtime_error(py::repr(lam).cast<std::string>());
                  const double la = lam.cast<double>();
                  tree_dual_gap(n,
                                gamma.mutable_data(),
                                x.data(),
                                alpha.data(),
                                la,
                                parent.data(),
                                root_val);
              }
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
}
