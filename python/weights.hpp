#pragma once
#include <string>
#include <graphidx/bits/weights.hpp>
#include "py_np.hpp"


inline void
check_len(const ssize_t /* n */,
          const double & /* a */,
          const std::string &)
{
}


inline Array<const double>
convert(const py::array_f64 &a)
{
    return Array<const double>(a.data());
}


inline Const<double>
convert(double a)
{
    return Const<double>(a);
}


inline Ones<double>
convert()
{
    return Ones<double>();
}
