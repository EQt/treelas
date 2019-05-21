#pragma once
#include <cmath>            // std::abs
#include <iomanip>          // std::setw
#include <sstream>          // for std::stringstream
#include <vector>

#include "event.hpp"
#include "range.hpp"


/** end is exclusive! */
template<typename Event = Event3>
std::string
pwl_csv(const Event3 *begin, const Event3 *end, const double margin = 1.0)
{
    double sumo = 0.0;
    for (auto it = begin; it != end; it++) {
        sumo += it->offset();
    }
    double
        offset = -sumo/2.0,
        slope = 0.0,
        x = begin->x - margin;

    std::stringstream res;
    auto out = [&] () {
        double y = (x * slope + offset);
        if (std::abs(y) < 1e-15)
            y = 0.0;
        res << std::showpos << x << ","
            << std::showpos << y << std::endl;
    };
    out();
    for (const Event3 *e = begin; e != end; e++) {
        x = e->x;
        offset += e->offset();
        slope += e->slope;
        out();
    }
    x += margin;
    out();
    return res.str();
}


/** Printout the knots given by events `els` in the range `q` */
template<typename Event = Event3>
std::string
pwl_csv(const Range &q, const Event3 *els)
{
    return pwl_csv(els + q.start, els + q.stop + 1);
}


template<typename Event = Event3>
std::string
pwl_csv(const Range &q, const std::vector<Event3> els)
{
    return pwl_csv(q, els.data());
}
