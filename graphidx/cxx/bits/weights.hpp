#pragma once
#include <cstddef>


template <typename T = double>
struct ConstantWeights
{
    const T c;

    ConstantWeights(const T &c) : c(c) { }

    T operator[](size_t) const { return c; }
};


template <typename T = double>
struct ArrayWeights
{
    const T *a;

    ArrayWeights(const T *a) : a(a) { }

    T operator[](size_t i) const { return a[i]; }
};


template <typename T, typename S>
T
create_weight(S);


template <typename T>
ConstantWeights<T>
create_weight(T c)
{
    return ConstantWeights<T>(c);
}


template <typename T>
ArrayWeights<T>
create_weight(const T *a)
{
    return ArrayWeights<T>(a);
}
