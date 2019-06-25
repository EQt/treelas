#pragma once
#include <iostream>         // std::ostream
#include <iomanip>          // std::setw

#include "event3.hpp"

/**
   Record the changes of a piecewise-linear function.

   The offset is calculated implicitly as we assume that the postion
   `e.x` of the event `e` is the root of the linear function, i.e.

        e.x * e.slope + e.offset() == 0
*/
struct Event
{
    double x;
    double slope;

    /// Zero event: does not change anything.
    Event() : Event(0., 0.) {}

    /// Default constructor.
    Event(double x, double slope) : x(x), slope(slope) {}

    /// For compatibility: ignore third argument
    Event(double x, double slope, double) : Event(x, slope) {}

    inline double offset() const { return -x * slope; }

    /// Printing precision.
    const static int _p = 5;

    /// Printing width.
    const static int _w = 6;

    inline bool operator==(const Event &o) const {
        return x == o.x && slope == o.slope;
    }

    inline bool operator<(const Event &other) const {
        return this->x < other.x;
    }
};


inline std::ostream&
operator<<(std::ostream &o, const Event &e)
{
    o << "Event("
      << std::setw(Event::_w) << std::setprecision(Event::_p) << e.x << ", "
      << std::setw(Event::_w) << std::setprecision(Event::_p) << e.slope << ", "
      << std::endl;
    return o;
}
