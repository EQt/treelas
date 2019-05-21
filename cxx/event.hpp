#pragma once
#include <iostream>         // std::ostream
#include <iomanip>          // std::setw


struct Event3
{
    double x;
    double slope;
    double _offset;

    /*  // Clang does not support syntax({.x = .3, .slope = 1.0, ._offset})
    Event() : Event(0, 0, 0) {}
    Event(double x, double s, double o) : x(x), slope(s), _offset(o) {}
    */

    inline double val() const { return x * slope + offset(); }
    inline double offset() const { return _offset; }

    const static int _p = 5;
    const static int _w = 6;

    inline bool operator==(const Event3 &o) const {
        return x == o.x && slope == o.slope && offset() == o.offset();
    }

    inline bool operator<(const Event3 &other) const {
        return this->x < other.x;
    }
};


inline std::ostream&
operator<<(std::ostream &o, const Event3 &e)
{
    o << "Event("
      << std::setw(Event3::_w) << std::setprecision(Event3::_p) << e.x << ", "
      << std::setw(Event3::_w) << std::setprecision(Event3::_p) << e.slope << ", "
      << std::setw(Event3::_w) << std::setprecision(Event3::_p) << e.offset() << ")"
      << std::endl;
    return o;
}


struct Event
{
    double x;
    double slope;

    Event() : Event(0., 0.) {}
    Event(double x, double slope, double) : Event(x, slope) {}
    Event(double x, double slope) : x(x), slope(slope) {}
    inline double offset() const { return -x * slope; }
    inline double val() const { return x * slope + offset(); }

    const static int _p = 5;
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
