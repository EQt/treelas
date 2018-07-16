// Prefer the C allocator (compared to the C++ new operator)
#pragma once
#include <cstdlib>

template<typename T>
struct Malloc
{
    size_t n;
    T *_x;
    Malloc(size_t n) : n(n) {
        _x = (T*) malloc(n * sizeof(T));
    }

    ~Malloc() {
        if (_x != nullptr)
            free(_x);
    }

    operator T*() { return _x; }
    operator const T*() const { return _x; }

    T* data() { return _x; }
    const T* data() const { return _x; }

    size_t size() const { return n; }
};
