/** bitstring(num).
 * Return a {0,1} string bit representation of `num`.
 */
#pragma once
#include <string>
#include <bitset>


template <typename T>
inline std::string
bitstring(T x)
{
    static_assert(std::is_integral<T>::value, "Integral required.");
    return std::bitset<8*sizeof(T)>(x).to_string();
}
