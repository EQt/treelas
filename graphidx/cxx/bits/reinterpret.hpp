#pragma once
#include <cstdint>


inline uint32_t
reinterpret_uint(float x)
{
    static_assert(sizeof(uint32_t) == sizeof(float), "Wrong architecture");

    union { float x; uint32_t u; } b;
    b.x = x;
    return b.u;
}


inline uint64_t
reinterpret_uint(double x)
{
    static_assert(sizeof(uint64_t) == sizeof(double), "Wrong architecture");

    union { double x; uint64_t u; } b;
    b.x = x;
    return b.u;
}

