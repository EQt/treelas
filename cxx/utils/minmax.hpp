#pragma once

template <typename float_ = double>
inline float_
min(float_ a, float_ b)
{
    return a < b ? a : b;
}


template <typename float_ = double>
inline float_
max(float_ a, float_ b)
{
    return a > b ? a : b;
}
