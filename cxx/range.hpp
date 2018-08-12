#pragma once
#include <iostream>

/**
   Queue is represented as an interval on an array
 */
struct Queue
{
    int start;      // first element's index (inclusive)
    int stop;       // last  element's index (inclusive! unlike Python)

    int length() const {
        return stop - start + 1;
    }

    bool operator==(const Queue &other) const {
        return this->start == other.start && this->stop == other.stop;
    }
};


inline std::ostream&
operator<<(std::ostream &o, const Queue &q)
{
    return o << q.start << ":" << q.stop;
}
