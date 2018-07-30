#pragma once
#include <sstream>          // for std::stringstream
#include <iomanip>          // std::setw

#include "../event.hpp"
#include "../queue.hpp"


/** end is exclusive! */
template<typename Event = Event>
std::string
pwl_csv(const Event *begin, const Event *end, const double margin = 1.0)
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
    for (const Event *e = begin; e != end; e++) {
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
template<typename Event = Event>
std::string
pwl_csv(const Queue &q, const Event *els)
{
    return pwl_csv(els + q.start, els + q.stop + 1);
}


template<typename Event = Event>
std::string
pwl_csv(const Queue &q, const std::vector<Event> els)
{
    return pwl_csv(q, els.data());
}
