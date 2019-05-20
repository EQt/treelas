#include <cmath>            // std::abs(double) -> double
#include "event.hpp"
#include "range.hpp"

/*
  The reason why there are two different versions of the clip_front method:

  In the end we usually divide by slope to obtain a position x, which might be problematic in case that slope is (almost) zero.
*/

#define SLOPE_EPS (1e-8)


template<typename E = Event>
inline double
clip_front(E *elements,
           Range &pq,
           double slope,
           double offset,
           const double t)
{
    const E *e = &elements[pq.start];
    while (pq.start <= pq.stop && slope * e->x + offset < t) {
        offset += e->offset();
        slope += e->slope;
        e = &elements[++pq.start];
    }
    double x = (t - offset)/slope;
    /* -t because the next time this event will be triggered,
       it will have included t in the offset */
    elements[--pq.start] = E({x, slope, offset -t});
    // e = elements[pq.start];
    // assert (e->x * e->slope + e->offset() == t)
    return x;
}


template<typename E = Event>
inline double
clip_back(E *elements,
          Range &pq,
          double slope,
          double offset,
          const double t)
{
    const E *e = &elements[pq.stop];
    while (pq.start <= pq.stop && slope * e->x + offset > t) {
        offset -= e->offset();
        slope -= e->slope;
        e = &elements[--pq.stop];
    }

    double x = (t - offset)/slope;
    elements[++pq.stop] = E({x, -slope, -offset + t});
    return x;
}


inline double
clip_fronw(
    Event *elements,
    Range &pq,
    double slope,
    double offset,
    const double t,
    const double lower_bound)
{
//    CLIP_START();
    const int start0 = pq.start;
    const Event *e = &elements[start0];
    while (pq.start <= pq.stop && slope * e->x + offset < t) {
        offset += e->offset();
        slope += e->slope;
        e = &elements[++pq.start];
    }

    if (std::abs(slope) < SLOPE_EPS) {
        return pq.start == start0 ? lower_bound : elements[--pq.start].x;
    }

    const double x = (t - offset)/slope;
    /* // this should never happen
    if (std::abs(slope) < SLOPE_EPS) {
        if (pq.start <= pq.stop) {
            x = (e-1)->x;
        } else {
            x = lower_bound;
        }
    }
    */
    /* -t because the next time this event will be triggered,
       it will have included t in the offset */
    elements[--pq.start] = Event({x, slope});

// #ifdef DEBUG
//     // if (x > upper_bound - t)
//     //     CLIP_THROW("clip_front: x is too big");
//     if (x < lower_bound + t)
//         CLIP_THROW("clip_front: x is too small");
// #endif
    return x;
}


inline double
clip_backw(
    Event *elements,
    Range &pq,
    double slope,
    double offset,
    const double t,
    const double upper_bound)
{
    const auto stop0 = pq.stop;
//    CLIP_START();
    const Event *e = &elements[stop0];
    while (pq.start <= pq.stop && slope * e->x + offset > t) {
        offset -= e->offset();
        slope -= e->slope;
        e = &elements[--pq.stop];
    }

    if (std::abs(slope) < SLOPE_EPS) {
        return pq.stop == stop0 ? upper_bound : elements[++pq.stop].x;
    }

    const double x = (t - offset)/slope;
    elements[++pq.stop] = Event({x, -slope});
// #ifdef DEBUG
//     if (x > upper_bound + t)
//         CLIP_THROW("clip_back: x is too big");
//     // if (x < lower_bound - t)
//     //     CLIP_THROW("clip_back: x is too big");
// #endif
    return x;
}
