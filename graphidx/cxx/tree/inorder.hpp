#pragma once
#include <vector>


template <typename I = int>
std::vector<I>
binary_inorder(const size_t height)
{
    std::vector<I> out;
    size_t n = (1 << height) -1;
    if (n <= 0)
        return std::vector<I>();
    out.resize(n);

    for  (size_t s = 2; n >= 1; n >>= 1, s <<= 1) {
        const size_t
            t = s >> 1,
            a = n >> 1,
            b = n - 1;
        for (size_t i = 1, j = a; j <= b; i++, j++) {
            // printf("out[%d] = %d\n", i*s - t - 1, j);
            out[i*s - t - 1] = I(j);
        }
    }

    return out;
}
