#pragma once

/** Similar to std::queue but with pre-allocated memory, i.e. size is limited */
template<typename T = int>
class queue
{
    size_t
        max = 0,        // maximum number of elements
        n = 0;          // number of inserted elements
    size_t head = 0;    // front element's index
    size_t tail = 0;    // index where to push the next element
    T *e = nullptr;     // elements

public:
    queue() { }

    queue(const size_t max_n) { reserve(max_n); }

    ~queue() { free(); }

    void free() {
        if (e) delete[] e;
        e = nullptr;
    }

    inline void clear() { head = tail = n = 0; }

    inline void reserve(size_t max_n) {
        if (max_n <= 0)
            throw std::invalid_argument(std::string("queue::reserve(") +
                                        std::to_string(n) + ")");
        if (this->max < max_n) {
            free();
            e = new T[max_n];
            this->max = max_n;
            clear();
        }
    }

    inline void push(T x) {
        e[tail++ % max] = x;
        n++;
    }

    inline T front() const { return e[head]; }

    inline void pop() { head = (head + 1) % max; n--; }

    inline bool empty() const { return n == 0; }

    inline size_t size() const { return n; }

    inline size_t capacity() const { return max; }

#ifdef GTEST_INCLUDE_GTEST_GTEST_PROD_H_
    FRIEND_TEST(queue, push1);
    FRIEND_TEST(queue, push2);
#endif

};

