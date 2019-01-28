/** bitstring(num).
 * Return a {0,1} string bit representation of `num`.
 */
#pragma once
#include <string>
#include <cstdint>
#include <bitset>
#include "reinterpret.hpp"


template <typename T>
inline std::string
bitstring(T x)
{
    static_assert(std::is_integral<T>::value, "Integral required.");
    return std::bitset<8*sizeof(T)>(x).to_string();
}


template<>
inline std::string
bitstring(float x)
{
    return bitstring(reinterpret_uint(x));
}


template<>
inline std::string
bitstring(double x)
{
    return bitstring(reinterpret_uint(x));
}
