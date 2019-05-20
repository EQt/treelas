#pragma once
#include <iostream>         // std::ostream
#include <iomanip>          // std::setw


struct Event
{
    double x;
    double slope;
    double _offset;
    // Event() : Event(0, 0, 0) {}
    /* Event(double x, double slope, double offset) :
       x(x), slope(slope), offset(offset) {} */
    // double extra[20] = {0.};
    inline double val() const { return x * slope + offset(); }

    inline double offset() const { return _offset; }

    const static int _p = 5;
    const static int _w = 6;

    bool operator==(const Event &o) const {
        return x == o.x && slope == o.slope && offset() == o.offset();
    }
};


struct Event2
{
    double x;
    double slope;
    Event2() : Event2(0., 0.) {}
    Event2(double x, double slope, double) : Event2(x, slope) {}
    Event2(double x, double slope) : x(x), slope(slope) {}
    inline double offset() const { return -x * slope; }
    inline double val() const { return x * slope + offset(); }

    const static int _p = 5;
    const static int _w = 6;

    bool operator==(const Event2 &o) const {
        return x == o.x && slope == o.slope;
    }
};


inline std::ostream&
operator<<(std::ostream &o, const Event &e)
{
    o << "Event("
      << std::setw(Event::_w) << std::setprecision(Event::_p) << e.x << ", "
      << std::setw(Event::_w) << std::setprecision(Event::_p) << e.slope << ", "
      << std::setw(Event::_w) << std::setprecision(Event::_p) << e.offset() << ")"
      << std::endl;
    return o;
}


inline std::ostream&
operator<<(std::ostream &o, const Event2 &e)
{
    o << "Event("
      << std::setw(Event2::_w) << std::setprecision(Event2::_p) << e.x << ", "
      << std::setw(Event2::_w) << std::setprecision(Event2::_p) << e.slope << ", "
      << std::endl;
    return o;
}


inline bool
operator<(const Event2 &e1, const Event2 &e2) {
    return e1.x < e2.x;
}


inline bool
operator<(const Event &e1, const Event &e2) {
    return e1.x < e2.x;
}
