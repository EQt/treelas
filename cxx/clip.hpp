#include <cmath>            // std::abs(double) -> double
#include "event.hpp"
#include "range.hpp"

/** Minimal slope after which the PWL is treated as having zero slope. */
#define SLOPE_EPS (1e-8)


/** Cut all knots until the PWL is at least `t`.

  The reason why there are two versions, `clip_front` and `clip_fronw`:

  In the end we usually divide by slope to obtain a position x, which
  might be problematic in case that slope is (almost) zero.
  Setting the position `x` to \f$-\infty\f$ will result in the problem that
  `x` times `slope` is not exactly `t`.

  Hence we agree that we will never insert an Event with zero slope.
  Zero-slope Events can occur only at the beginning or the end of a queue.
  That is way they can be merged into the adjoining events.
  Just bear in mind that the initial offset might change by this change.
  That is way I started two different implementations.

  In case I favor a solution that returns the initial `offset` and the
  position `x` the method
  <a href="http://www.cplusplus.com/reference/tuple/tie/">std::tie</a>
  might be helpful.

  ## Usage

  This methods returns the position where the event will happen next time.
  Furthermore, a corresponding event is prepended to `elements`.

  ## Why do we need the position `x` at all?

  There are mainly to occasions: We need it to
   1. order the events
   2. have a proper bound in case slope is not zero

  Regarding the first point, the ordering: In principle it is possible
  to avoid the division and store `(offset, slope)` instead of `(x, slope)`.
  However, to compare two Event s we would need to do a multiplication
  each time.

  Also for the second point we could avoid the division if we store
  the slope and offset instead of the position `x`.

  All in all I realized that events with zero-slope can be avoided:
  they simple don't have an effect on further processing.

  @see `clip_fronw`
*/
template<typename E = Event>
inline double
clip_front(E *elements,
           Range &pq,
           double slope,
           double offset)
{
    const E *e = &elements[pq.start];
    while (pq.start <= pq.stop && slope * e->x + offset < 0) {
        offset += e->offset();
        slope += e->slope;
        e = &elements[++pq.start];
    }
    double x = -offset/slope;
    /* -t because the next time this event will be triggered,
       it will have included t in the offset */
    elements[--pq.start] = E({x, slope, offset});
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
