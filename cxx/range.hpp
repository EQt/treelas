#pragma once
#include <iostream>

/**
   Range is represented as an interval on an array
 */
struct Range
{
    int start;      // first element's index (inclusive)
    int stop;       // last  element's index (inclusive! unlike Python)

    int length() const {
        return stop - start + 1;
    }

    bool operator==(const Range &other) const {
        return this->start == other.start && this->stop == other.stop;
    }
};


inline std::ostream&
operator<<(std::ostream &o, const Range &q)
{
    return o << q.start << ":" << q.stop;
}
