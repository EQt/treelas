#pragma once

#ifndef _STACK_DEBUG
#  define _STACK_DEBUG 0
#else
#  if !(_STACK_DEBUG)
#     error  "NO DEBUG!!"
#  endif
#endif
#ifndef STACK_HPP
#  define STACK_HPP
#  include <string>
#  include <stdexcept>


template<typename T = int>
class stack
{
    size_t n = 0;
    size_t pos = 0;     // points to the next element

    T *e = nullptr;

public:
    inline static bool debug_mode() {
        #if _STACK_DEBUG
        return true;
        #else
        return false;
        #endif
    }

    stack() { }

    stack(const size_t n) { reserve(n); }

    ~stack() { free(); }

    void free() {
        #if _STACK_DEBUG
        if (pos > n)
            throw std::overflow_error("stack::clear()");
        #endif
        if (e) delete[] e;
        e = nullptr;
        pos = 0;
    }

    inline void clear() { pos = 0; }

    inline void reserve(size_t n) {
        if (n <= 0)
            throw std::invalid_argument(std::string("stack::reserve(") +
                                        std::to_string(n) + ")");
        if (this->n < n) {
            free();
            e = new T[n];
            this->n = n;
            this->pos = 0;
        }
    }

    inline void push_back(T x) {
        #if _STACK_DEBUG
        if (pos >= n)
            throw std::overflow_error("stack::push_back");
        #endif
        e[pos++] = x;
    }

    inline T back() const {
        #if _STACK_DEBUG
        if (empty()) {
            throw std::underflow_error("stack::back()");
        }
        #endif
        return e[pos-1];
    }

    inline T pop_back() {
        #if _STACK_DEBUG
        if (empty()) {
            throw std::underflow_error("stack::pop_back()");
        }
        #endif
        return e[--pos];
    }

    inline bool empty() const { return pos <= 0; }

    inline size_t size() const { return pos; }

    inline size_t capacity() const { return n; }

    inline void swap(const size_t i, const size_t j) { std::swap(e[i], e[j]); }

#ifdef GTEST_INCLUDE_GTEST_GTEST_PROD_H_
    FRIEND_TEST(stack, underflow0);
#endif
};


#endif // STACK_HPP
