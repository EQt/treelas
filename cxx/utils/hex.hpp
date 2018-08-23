#pragma once
#include <string>
#include <cstring>      // for memcpy


inline std::string
hexify(uint64_t x)
{
    char h [2*sizeof(x) + 3 + 3 + 2];
    snprintf(h, sizeof(h), "d64(0x%016llx)", x);
    return std::string(h);
}


inline std::string
hex(double x)
{
    static_assert(sizeof(uint64_t) == sizeof(double), "Wrong architecture");
    uint64_t i;
    memcpy(&i, &x, sizeof(uint64_t));
    return hexify(i);
}
