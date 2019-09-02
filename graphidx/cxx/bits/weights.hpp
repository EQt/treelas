#include <cstddef>


template <typename T>
struct ConstantWeights
{
    const T c;

    ConstantWeights<T>(const T &c) : c(c) { }

    T operator[](size_t) const { return c; }
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
