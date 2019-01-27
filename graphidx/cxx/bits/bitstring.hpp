/** bitstring(num).
 * Return a {0,1} string bit representation of `num`.
 */
#pragma once
#include <string>
#include <cstdint>
#include <bitset>


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
    union { float x; uint32_t u; } b;
    b.x = x;
    return bitstring(b.u);
}


template<>
inline std::string
bitstring(double x)
{
    union { double x; uint64_t u; } b;
    b.x = x;
    return bitstring(b.u);
}
