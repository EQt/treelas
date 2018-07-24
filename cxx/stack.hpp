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
public:
    size_t n = 0;
    T *e = nullptr;
    long int pos = -1;

public:
    inline static bool debug_mode() {
        #if _STACK_DEBUG
        return true;
        #else
        return false;
        #endif
    }

    stack() { }

    ~stack() { free(); }

    void free() {
        #if _STACK_DEBUG
        if (pos > n-1)
            throw std::overflow_error("stack::clear()");
        #endif
        if (e)
            delete[] e;
        e = nullptr;
        pos = -1;
    }

    void clear() {
        pos = -1;
    }

    void reserve(size_t n) {
        if (n <= 0)
            throw std::invalid_argument(std::string("stack::reserve(") +
                                        std::to_string(n) + ")");
        if (this->n < n) {
            free();
            e = new T[n];
            this->n = n;
        }
    }

    inline void push_back(T x) {
        #if _STACK_DEBUG
        if (pos > n-2)
            throw std::overflow_error("stack::push_back");
        #endif
        e[++pos] = x;
    }

    inline T back() const {
        #if _STACK_DEBUG
        if (empty()) {
            throw std::underflow_error("stack::back()");
        }
        #endif
        return e[pos];
    }

    inline T pop_back() {
        #if _STACK_DEBUG
        if (empty()) {
            throw std::underflow_error("stack::pop_back()");
        }
        #endif
        return e[pos--];
    }

    inline bool empty() const {
        return pos < 0;
    }

    inline size_t size() const {
        return pos + 1;
    }
};


#endif // STACK_HPP
