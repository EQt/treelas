#include <cmath>            // std::abs(double) -> double
#include <limits>           // infinity

#include <graphidx/utils/viostream.hpp>

#include "event.hpp"
#include "range.hpp"

/** Minimal slope after which the PWL is treated as having zero slope. */
#define SLOPE_EPS (1e-8)


static const auto EPS = 1e-10;
static const auto DEBUG = false;

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

  The only that can happen is that the associated edge edge might shrink
  the possible flow (or overtake the effect of the proceeding other edges).
  That is why I would like to return both, x and initial offset while
  the zero slope should not go into the queue.


  # Checking `abs(slope) > eps`

  When is this really necessary?

  It cannot happen if we never insert a line with `slope == 0`.
  But can it happen at other places after some events have been processed?

  What does it mean?
  The implication is that a node's value is not unique.
  Of course that can also happen for "inner nodes" with non-latent children, e.g.

  ```
  (V) -- (L) -- (V)

  L: latent
  V: non-latent
  ```
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
