#pragma once

/** Similar to std::queue but with pre-allocated memory, i.e. size is limited */
template<typename T = int>
class queue
{
    size_t n = 0;       // total number of elements
    size_t head = 0;    // front element's index
    size_t tail = 0;    // back element's index
    T *e = nullptr;     // elements

public:
    queue() { }

    queue(const size_t n) { reserve(n); }

    ~queue() { free(); }

    void free() {
        if (e) delete[] e;
        e = nullptr;
    }

    inline void clear() { head = tail = 0; }

    inline void reserve(size_t n) {
        if (n <= 0)
            throw std::invalid_argument(std::string("queue::reserve(") +
                                        std::to_string(n) + ")");
        if (this->n < n) {
            free();
            e = new T[n];
            this->n = n;
            clear();
        }
    }

    inline void push(T x) {
        e[tail] = x;
        tail = (tail + 1) % n;
    }

    inline T front() const { return e[head]; }

    inline void pop() { head = (head + 1) % n; }

    inline bool empty() const { return head == tail; }

    inline size_t size() const { return head <= tail ? tail - head : tail - head + n; }

    inline size_t capacity() const { return n; }

#ifdef GTEST_INCLUDE_GTEST_GTEST_PROD_H_
    FRIEND_TEST(queue, push1);
    FRIEND_TEST(queue, push2);
#endif

};

