#pragma once
#include <algorithm>        // for std::min_element


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


template<typename float_ = float,
         typename int_ = int>
inline void
find_minmax(const float_ *y,
            const int_ n,
            float_ &y_min, float_ &y_max)
{
    y_min = *std::min_element(y, y + n);
    y_max = *std::max_element(y, y + n);
}


template<typename float_ = float>
inline float_
clip(float_ x, float_ a, float_ b)
{
    /*
    if (x >= b)
        return b;
    if (x <= a)
        return a;
    return x;
    */
    return x >= b ? b : (x <= a ? a : x);
}
