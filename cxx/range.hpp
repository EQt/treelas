#pragma once
#include <iostream>

/**
   Range is an interval on an array (inclusive boundaries!).
   They represent double-ended priority queues.
 */
struct Range
{
    int start;      // first element's index (inclusive)
    int stop;       // last  element's index (inclusive! unlike Python)

    size_t length() const { return stop - start + 1; }

    bool operator==(const Range &other) const {
        return this->start == other.start && this->stop == other.stop;
    }

    operator bool() const { return start <= stop; }
};


inline std::ostream&
operator<<(std::ostream &o, const Range &q)
{
    return o << q.start << ":" << q.stop;
}
