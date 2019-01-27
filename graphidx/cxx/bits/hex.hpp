/** hex(number).
 *
 * Return the hexadecimal representation of a number.
 * For floating point numbers, the string will be prefixed by the Rust
 * type name, i.e.
 * ```c++
 * hex(1.0f) == "f32(0x3f800000);
 * ```
 */
#pragma once
#include <string>
#include <cinttypes>    // for PRIx64
#include "reinterpret.hpp"


inline std::string
hex(uint64_t x, const char *prefix = nullptr)
{
    char h [2*sizeof(x) + 3 + 3 + 2];
    if (prefix)
        snprintf(h, sizeof(h), "%s(0x%0" PRIx64 ")", prefix, x);
    else
        snprintf(h, sizeof(h), "0x%0" PRIx64, x);
    return std::string(h);
}


inline std::string
hex(int64_t x)
{
    char h [2*sizeof(x) + 3 + 3 + 2];
    snprintf(h, sizeof(h), "0x%0" PRIx64, x);
    return std::string(h);
}


inline std::string
hex(uint32_t x, const char *prefix = nullptr)
{
    char h [2*sizeof(x) + 3 + 3 + 2];
    if (prefix)
        snprintf(h, sizeof(h), "%s(0x%0" PRIx32 ")", prefix, x);
    else
        snprintf(h, sizeof(h), "0x%0" PRIx32, x);
    return std::string(h);
}


inline std::string
hex(int32_t x)
{
    char h [2*sizeof(x) + 3 + 3 + 2];
    snprintf(h, sizeof(h), "0x%0" PRIx32, x);
    return std::string(h);
}


inline std::string
hex(double x)
{
    return hex(reinterpret_uint(x), "f64");
}


inline std::string
hex(float x)
{
    return hex(reinterpret_uint(x), "f32");
}
