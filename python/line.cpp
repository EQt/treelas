#include <pybind11/pybind11.h>
#include <typeinfo>
#include "py_np.hpp"
#include "weights.hpp"

#include "../deps/glmgen/tf.hpp"
#include "../deps/condat/condat_tv_v2.hpp"

#include "../cxx/line_dp.hpp"
#include "../cxx/line/line_para.hpp"
#include "../cxx/line/line_c.hpp"
#include "../cxx/line/line_c2.hpp"
#include "../cxx/line/line_c3.hpp"


namespace py = pybind11;


template <typename LamFrom = py::array_f64&,
          typename LamTo = ArrayWeights<double>,
          typename MuFrom = py::array_f64&,
          typename MuTo = ArrayWeights<double>,
          bool CHECK = true>
void
reg_line_las(py::module &m, const char *doc = "")
{
    m.def("line_dp",
          [](const py::array_f64 &y,
             const LamFrom lam,
             const MuFrom mu,
             py::array_f64 &x,
             const bool verbose) -> py::array_t<double>
          {
              printf("check = %s: %s %f\n", CHECK ? "true" : "false",
                     typeid(MuTo).name(),
                     convert(mu)[0]);
              fflush(stdout);
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(y);
              check_len(n-0, mu, "mu1");
              check_len(n-1, lam, "lam");
              if (is_empty(x))
                  x = py::array_f64({n}, {sizeof(double)});
              check_len(n, x, "x");
              {
                  line_las<double, LamTo, MuTo, CHECK>(
                      n,
                      x.mutable_data(),
                      y.data(),
                      convert(lam),
                      convert(mu)
                  );
              }
              return x;
          },
          doc,
          py::arg("y"),
          py::arg("lam"),
          py::arg("mu") = 1,
          py::arg("x") = py::none(),
          py::arg("verbose") = false
        );
}


void
reg_line(py::module &m)
{
    m.def("line_condat", [](const py::array_f64 &y,
                            const double lam,
                            py::array_f64 out)
          -> py::array_t<double> {
              const auto n = check_1d_len(y, "y");
              if (is_empty(out))
                  out = py::array_t<double>({{n}}, {{sizeof(double)}});
              check_len(n, out, "out");
              TV1D_denoise_v2(y.data(),
                              out.mutable_data(),
                              (unsigned int)n,
                              lam);
              return out;
          },
          R"pbdoc(
            Line solver, implemented by Laurent Condat,
            version 2.0, Aug. 30, 2017.
            See: https://www.gipsa-lab.grenoble-inp.fr/~laurent.condat

            Memory usage: 2*len(y)*sizeof(uint32_t)
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none());

    m.def("line_glmgen", [](const py::array_f64 &y,
                            const double lam,
                            py::array_f64 out,
                            const bool verbose) -> py::array_t<double>
          {
#ifdef HAVE_GLMGEN
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(y, "y");
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");
              glmgen::tf_dp(int(n),
                            y.data(),
                            lam,
                            out.mutable_data());
              return out;
#else
              PyErr_SetString(PyExc_NotImplementedError,
                              "glmgen/tf_dp.c was not available");
              return y;
#endif
          },
          R"pbdoc(
`            Line solver (implementation of the R package `glmgen`).

            Memory: ??*len(y)*sizeof(uint32_t)
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none(),
          py::arg("verbose") = false);

    m.def("line_lasc",
          [](const py::array_f64 &y,
             const double lam,
             py::array_f64 out,
             const bool verbose) -> py::array_t<double>
          {
              TimerQuiet _ (verbose);
              const int n = int(check_1d_len(y, "y"));
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");
              dp_line_c(n,
                        y.data(),
                        lam,
                        out.mutable_data());
              return out;
          },
          R"pbdoc(
            Line solver (own implementation, hand tuned C code).

            Memory: ??*len(y)*sizeof(uint32_t)
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none(),
          py::arg("verbose") = false);

    m.def("line_las2",
          [](const py::array_f64 &y,
             const double lam,
             py::array_f64 out,
             const bool verbose) -> py::array_t<double>
          {
              TimerQuiet _ (verbose);
              const int n = int(check_1d_len(y, "y"));
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");
              dp_line_c2(n,
                         y.data(),
                         lam,
                         out.mutable_data());
              return out;
          },
          R"pbdoc(
            Line solver (own implementation, save more memory compared to line_lasc)

            Memory: ??*len(y)*sizeof(uint32_t)
          )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none(),
          py::arg("verbose") = false);

    m.def("line_las3",
          [](const py::array_f64 &y,
             const double lam,
             py::array_f64 out,
             const bool verbose) -> py::array_t<double>
          {
              TimerQuiet _ (verbose);
              const int n = int(check_1d_len(y, "y"));
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");
              dp_line_c3(n,
                         y.data(),
                         lam,
                         out.mutable_data());
              return out;
          },
          R"pbdoc(
                  Line solver (own implementation, save more memory compared to line_lasc).

                  Memory: ??*len(y)*sizeof(uint32_t)
              )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none(),
          py::arg("verbose") = false);

    m.def("line_para",
          [](const py::array_f64 &y,
             const double lam,
             py::array_f64 out,
             const bool verbose,
             const bool parallel) -> py::array_t<double>
          {
              TimerQuiet _ (verbose);
              const int n = int(check_1d_len(y, "y"));
              if (is_empty(out))
                  out = py::array_t<double>({n}, {sizeof(double)});
              check_len(n, out, "out");

              line_para(n, y.data(), lam, out.mutable_data(), parallel);
              return out;
          },
          R"pbdoc(
                  Line solver (parallel from both ends).

                  Memory: ??*len(y)*sizeof(uint32_t)
              )pbdoc",
          py::arg("y"),
          py::arg("lam"),
          py::arg("out") = py::none(),
          py::arg("verbose") = false,
          py::arg("parallel") = false);


    m.def("line_dp",
          [](const py::array_f64 &y,
             const double lam,
             py::array_f64 &x,
             const bool verbose) -> py::array_t<double>
          {
              TimerQuiet _ (verbose);
              const auto n = check_1d_len(y);
              check_len(n-1, lam, "lam");
              if (is_empty(x))
                  x = py::array_f64({n}, {sizeof(double)});
              check_len(n, x, "x");
              {
                  line_las(n,
                           x.mutable_data(),
                           y.data(),
                           convert(lam));
              }
              return x;
          },
          "",
          py::arg("y"),
          py::arg("lam"),
          py::arg("x") = py::none(),
          py::arg("verbose") = false
        );

    reg_line_las<double, ConstantWeights<double>,
                 double, ConstantWeights<double>, false>(m);

    reg_line_las<py::array_f64&, ArrayWeights<double>,
                 double, ConstantWeights<double>, false>(m);

    reg_line_las<double, ConstantWeights<double>,
                 py::array_f64&, ArrayWeights<double>, true>(m);

    reg_line_las<py::array_f64&, ArrayWeights<double>,
                 py::array_f64&, ArrayWeights<double>, true>(m,
            R"pbdoc(
                Line solver (weights).

                Memory: ??*len(y)*sizeof(uint32_t)
            )pbdoc");
}
