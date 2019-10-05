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


inline ArrayWeights<double>
convert(const py::array_f64 &a)
{
    return ArrayWeights<double>(a.data());
}


inline ConstantWeights<double>
convert(double a)
{
    return ConstantWeights<double>(a);
}
