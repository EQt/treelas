#pragma once
#include <cmath>            // std::abs(double) -> double
#include <limits>           // infinity
#include <vector>

#include "event.hpp"
#include "range.hpp"

#ifndef DEBUG_CLIP
#  define DEBUG_CLIP false
#endif

/** Minimal slope after which the PWL is treated as having zero slope. */
static const double EPS = 1e-10;

/** Printout debugging information? */
static const bool DEBUG = DEBUG_CLIP;


/** Cut all knots until the PWL is at least zero.

    Hereby, start with initial `slope` and `offset`.
    To achieve a value of `t`, subtract it from `offset`.

    If `step` is positive, go from `pq.start` till `pq.stop` (inclusive);
    otherwise go the other direction, i.e.
    from `pq.stop` down to `pq.start` (inclusive).

    If `need_check` is set, ensure that in the end `std::abs(slope) > EPS`;
    if `slope` is too close to zero (i.e. `EPS`) the division by `slope`
    might result in strange behavior.
    That is why a new `Event` is only added if `std::abs(slope) > EPS`.
*/
template<int step, bool need_check = false, typename float_ = double>
inline float_
clip(Event *elem,
     Range &pq,
     float_ slope,
     float_ offset)
{
    constexpr auto dir = step > 0 ? "f" : "b";
    if (DEBUG) {
        printf("clip_%s: (%+g, %+.2f)\n", dir, slope, offset);
    }
    while (pq && slope * elem[step > 0 ? pq.start : pq.stop].x + offset < 0) {
        const Event &e = elem[step > 0 ? pq.start++ : pq.stop--];
        offset += e.offset();
        slope += e.slope;
        DEBUG && printf(" lip_%s: (%+g, %+.2f)\n", dir, slope, offset);
    }
    if (need_check && std::abs(slope) <= EPS)
        return step > 0 ?
            -std::numeric_limits<float_>::infinity() :
            +std::numeric_limits<float_>::infinity();
    const auto x = -offset/slope;
    elem[step > 0 ? --pq.start : ++pq.stop] = Event({x, slope});
    DEBUG && printf("  ip_%s: (%+g, %+.2f)\n", dir, slope, offset);
    return x;
}


template<int step, bool need_check = false, typename float_ = double>
inline float_
clip(std::vector<Event> &elem,
     Range &pq,
     float_ slope,
     float_ offset)
{
    return clip<step, need_check, float_>(elem.data(), pq, slope, offset);
}


[[deprecated]]
inline double
clip_front(Event *elements,
           Range &pq,
           double slope,
           double offset)
{
    return clip<+1, false>(elements, pq, slope, offset);
}


[[deprecated]]
inline double
clip_back(Event *elements,
          Range &pq,
          double slope,
          double offset,
          const double t)
{
    return clip<-1, false>(elements, pq, -slope, -offset + t);
}


[[deprecated]]
inline double
clip_fronw(
    Event *elements,
    Range &pq,
    double slope,
    double offset,
    const double t,
    const double /* lower_bound */)
{
    return clip<+1, true>(elements, pq, slope, offset - t);
}


[[deprecated]]
inline double
clip_backw(
    Event *elements,
    Range &pq,
    double slope,
    double offset,
    const double t,
    const double /* upper_bound */)
{
    return clip<-1, true>(elements, pq, -slope, -offset + t);
}


#undef DEBUG_CLIP
