#pragma once
#include <stdexcept>
#include <type_traits>      // std::remove_const


/**
   Allocate a memory block (if not provided).
 */
template<typename T>
class VecAlloc
{
    T *els = nullptr;
    T **other = nullptr;
    using alloc_t = typename std::remove_const<T>::type;
public:
    VecAlloc(T **p, size_t n) {
        if (p == nullptr)
            throw std::invalid_argument("Nullpointer");
        other = p;
        if (*p == nullptr) {
            els = new alloc_t[n];
            *other = els;
        }
    }

    ~VecAlloc() {
        if (els) {
            delete[] els;
            /// -Wterminate warning
            // if (!other)
            //     throw std::runtime_error("This should not happen");
            if (other) 
                *other = nullptr;
        }
    }
};
