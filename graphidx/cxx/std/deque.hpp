#pragma once
#include <vector>


/***
 *  Double ended queue.
**/
template <typename E>
struct DeQue
{
private:
    std::vector<E> _e;
    size_t front = 1, back = 0;

public:
    DeQue() {}
    DeQue(size_t n) { reserve(n); }

    /// allocate space for 2*n elements; resetting to the middle
    void reserve(const size_t n) { _e.reserve(2*n); front = n; back = n-1; }

    /// first or last element (depending on forward)
    template <bool forward = true>
    const E& peek() const { return forward ? _e[front] : _e[back]; }

    /// remove first/last element
    template <bool forward = true>
    E pop() { return forward ? _e[front++] : _e[back--]; }

    /// append an element to front/back of the queue
    template <bool forward = true>
    void push(E x) { if (forward) _e[--front] = x; else _e[++back] = x; }

    /// is the queue empty?
    operator bool() const { return front <= back; }

    /// number of elements present at the moment
    size_t size() const { return back - front + 1; }

    /// number of elements that could fit in
    size_t capacity() const { return _e.capacity(); }
};
