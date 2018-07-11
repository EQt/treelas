#pragma once
#include <pybind11/numpy.h>

namespace py = pybind11;

/*
template<typename T = double>
py::array<T>
create_array(T *x, size_t n)
{
    np::dtype dtype = np::dtype::get_builtin<T>();
    py::tuple shape = py::make_tuple(n);
    py::tuple stride = py::make_tuple(sizeof(T));
    auto owner = py::object();
    auto a = np::from_data(x, dtype, shape, stride, owner);
    return a;
}


template<typename T>
py::array<T>
empty_array()
{
    const np::dtype dtype = np::dtype::get_builtin<T>();
    const py::tuple shape = py::make_tuple();
    return np::zeros(shape, dtype);
}


inline bool
is_empty(const np::ndarray &a)
{
    for (int d = 0; d < a.get_nd(); d++) {
        if (a.shape(d) > 0) {
            return false;
        }
    }
    return true;
}


template<typename T = double>
inline np::ndarray
copy_vector(const std::vector<T> &x)
{
    const auto n = x.size();
    double *xdat = new double[n];
    std::memcpy(xdat, x.data(), sizeof(double) * n);
    return create_array(xdat, n);
}
*/

np::array<double>
test_create_array()
{
    double *x = new double[3];
    x[0] = 13.0;
    x[1] = -1.0;
    x[2] = 42.0;
    return create_array(x, 3);
}


template<typename T = double>
inline T*
check_data_mut(const np::ndarray &var, const std::string &var_str = "?")
{
    np::dtype dtype = np::dtype::get_builtin<T>();
    const auto vtype = var.get_dtype();
    if (vtype != dtype) {
        const std::string
            dtype_str = py::extract<std::string>(py::str(dtype)),
            vtype_str = py::extract<std::string>(py::str(vtype));
        throw std::invalid_argument(var_str + ".dtype = numpy." + vtype_str +
                                    " != numpy." + dtype_str);
    }
    if (var.strides(0) != sizeof(T)) {
        throw std::invalid_argument(var_str + ": wrong stride");
    }
    return (T*) var.get_data();
}


template<typename T = double>
inline const T*
check_data(const np::ndarray &var, const std::string &var_str = "?")
{
    return (const T*) check_data_mut<T>(var, var_str);
}


inline void
check_len(const int n,
          const np::ndarray &var,
          const std::string &var_str = "?",
          const int ndim = 1)
{
    if (var.get_nd() != ndim) {
        throw std::length_error(std::string("len(") +
                                var_str + ".shape) != " +
                                std::to_string(ndim));
    }
    if (var.shape(0) != n) {
        throw std::length_error(std::to_string(var.shape(0)) +
                                std::string(" = len(") +
                                var_str + ") != " + std::to_string(n));
    }
}


/** Be sure that an array is one-dimensional */
inline int
check_1d_len(const np::ndarray &a, const std::string &a_str = "?")
{
    if (a.get_nd() != 1) {
        throw std::length_error(a_str +
                                " is supposed to be 1-dimensional array");
    }
    return a.shape(0);
}
