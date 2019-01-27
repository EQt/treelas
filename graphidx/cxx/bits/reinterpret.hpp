#pragma once
#include <cstdint>


inline uint32_t
reinterpret_uint(float x)
{
    union { float x; uint32_t u; } b;
    b.x = x;
    return b.u;
}


inline uint64_t
reinterpret_uint(double x)
{
    union { double x; uint64_t u; } b;
    b.x = x;
    return b.u;
}

