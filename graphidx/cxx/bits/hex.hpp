#pragma once
#include <string>
#include <cstring>      // for memcpy
#include <cinttypes>    // for PRIx64


/** hex(number).
 * Return the hexadecimal representation of a number.
 */
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
hex(uint32_t x)
{
    char h [2*sizeof(x) + 3 + 3 + 2];
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
    static_assert(sizeof(uint64_t) == sizeof(double), "Wrong architecture");
    uint64_t i;
    memcpy(&i, &x, sizeof(uint64_t));
    return hex(i, "f64");
}
