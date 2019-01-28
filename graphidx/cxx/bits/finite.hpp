/**
  Ensure that all elements in a floating point array have finite value.
*/
#pragma once
#include <cmath>
#include <stdexcept>    // std::runtime_error
#include <string>       // std::to_string
#include <vector>

/** Find the index of the first non-finite (+/-Inf or NaN) element or return n */
inline size_t
find_non_finite(const double *a, const size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (!std::isfinite(a[i])) {
            return i;
        }
    }
    return n;
}


/** Throw std::runtime_error if `a` contains non-finite elements. */
inline void
check_all_finite(const double *a, const size_t n, const std::string &s = "?")
{
    const size_t i = find_non_finite(a, n);
    if (i < n) {
        throw std::runtime_error(s + "[" + std::to_string(i) +
                                 "] = " + std::to_string(a[i]) +
                                 " isfinite=" +
                                 std::to_string(std::isfinite(a[i])));
    }

}


/* for convinience ... */
inline void
check_all_finite(const std::vector<double> &a, const std::string &s = "?")
{
    check_all_finite(a.data(), size_t(a.size()), s);
}
