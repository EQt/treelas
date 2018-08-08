/* DEBUGGING UTILS

 Observe, that the events in the queue will only be changed in
 the last step when a new event is inserted and when the events are
 merged into the parent's queue.  That is why you can always debug
 the events and see how it ended at a certain value.

 clip_front: Critical questions
 ------------------------------

 1. When do I need to insert an Event?

 --> If the PWL is actually clipped, i.e. it has been once below
 and is now below t (=-lambda_i).

 2. How is x caclculated?

 In the case of an actually clipping, the x has to be calculated,
 either by division if the slope > 0, or by taking the event->x
 before (or after, does not matter as long as the slope does not
 change).

 If the PWL did not cross, i.e. is above from the very beginning
 (that includes the case of a leaf-node), we have to take the
 general lower_bound for x.
*/
#pragma once
#define DEBUG 0

#include "event.hpp"
#include "queue.hpp"
#include "utils/hex.hpp"


#ifdef DEBUG
#  define CLIP_START()  const Queue pq0 (pq); const double slope0 = slope;
#  define CLIP_THROW(msg)  if (pq.start > 40) {                     \
      std::ostringstream io;                                        \
      io << __FILE__ << ": " << __LINE__ << ": INTERNAL ERROR:  "   \
         << std::endl << msg << ":";                                \
      io << "t = " << t << " = " << hex(t) << std::endl;            \
      debug_dump(io, e, pq0, slope0, pq,                            \
                 slope, offset, t, x, elements);                    \
      throw std::runtime_error(io.str());                           \
    }
#  define EVENT_REC(Ev)                             \
    const Queue q0 (pq[i]);                         \
    const double offset0 = -mu[i]*y[i] -sigi;       \
    std::vector<Ev> ev0 (elements + q0.start,       \
                         elements + q0.stop+1);
#  define EVENT_CHECK(idx, _x) {                                        \
        const auto *e = &elements[idx];                                 \
        const double te = e->val();                                     \
        if (pq[i].start <= pq[i].stop &&                                \
            (std::abs(te) > 1e-8 /*|| _x != e->x */)) {                 \
            std::ostringstream io;                                      \
            io << __FILE__ << ": " << __LINE__                          \
               << ": INTERNAL ERROR:  " << std::endl;                   \
            io << "x       = " << _x                                    \
               << " = " << hex(_x) << std::endl                         \
               << "e->x    = " << e->x << std::endl;                    \
            io << "offset0 = " << offset0                               \
               << " = " << hex(offset0) << std::endl;                   \
            io << "i       = " << i << std::endl                        \
               << "y[i]    = " << y[i] << std::endl                     \
               << "mu[i]   = " << mu[i] << std::endl                    \
               << "sigi    = " << sigi                                  \
               << " = " << hex(sigi) << std::endl                       \
               << "lam[i]  = " << lam[i]                                \
               << " = " << hex(lam[i]) << std::endl;                    \
            io << "te      = " << te << " = " << hex(te)                \
               << std::endl;                                            \
            io << " e      = " << *e;                                   \
            io << " q0     = " << q0 << std::endl;                      \
            io << " pq[i]  = " << pq[i] << std::endl;                   \
            const int                                                   \
                a = std::min(q0.start, pq[i].start),                    \
                b = std::max(q0.stop, pq[i].stop);                      \
            io << "elements = {" << std::endl;                          \
            for (int j = a; j <= b; j++) {                              \
                io << " " << j << ": " << elements[j];                  \
            }                                                           \
            for (int j = 0; j <= b-a; j++) {                            \
                io << " " << j << ": " << ev0[j];                       \
            }                                                           \
            io << "}" << std::endl << "as hex(double):" << std::endl;   \
            dump_events(io, elements, a, b);                            \
            dump_events(io, ev0, 0, b-a);                               \
                                                                        \
            throw std::runtime_error(io.str());                         \
        }                                                               \
    }



#else
#  define CLIP_START() {}
#  define CLIP_THROW(msg) {}
#  define EVENT_REC() {}
#  define EVENT_CHECK(idx) {}
#endif


inline void
dump_events(std::ostream &io,
            const Event *elements,
            const int a,
            const int b)
{
    for (int i = a; i <= b; i++) {
        const Event &e = elements[i];
        if (i == a)
            io << "elements = {" << std::endl;
        else
            io << "," << std::endl;
        io << " " << i << ": Event("
           << hex(e.x) << ", "
           << hex(e.slope) << ", "
           << hex(e.offset()) << ")";
    }
    io << std::endl << "}" << std::endl;
}


inline void
dump_events(std::ostream &io,
            const Event2 *elements,
            const int a,
            const int b)
{
    for (int i = a; i <= b; i++) {
        const Event2 &e = elements[i];
        if (i == a)
            io << "elements = {" << std::endl;
        else
            io << "," << std::endl;
        io << " " << i << ": Event("
           << hex(e.x) << ", "
           << hex(e.slope) << ", "
           << hex(e.offset()) << ")";
    }
    io << std::endl << "}" << std::endl;
}



template<typename E = Event>
inline void
dump_events(std::ostream &io,
            const std::vector<E> &elements,
            const int a,
            const int b)
{
    return dump_events(io, elements.data(), a, b);
}



template<typename E = Event>
inline void
dump_events(std::ostream &io,
            const E *elements,
            const Queue &pq)
{
    dump_events(io, elements, pq.start, pq.stop);
}


inline void
dump_events(std::ostream &io,
            const std::vector<Event> &elements,
            const Queue &pq)
{
    return dump_events(io, elements.data(), pq);
}


/** Read into python: see dpdebug.py */
template<typename E = Event>
inline void
debug_dump(std::ostream &io,
           const E *e,
           const Queue &pq0,
           const double slope0,
           const Queue &pq,
           const double slope,
           const double offset,
           const double t,
           const double x,
           const E *elements)
{
    io << "x=" << x << std::endl
       << "e=&elements[" << int(e - elements) << "]" << std::endl
       << "slope=" << slope << std::endl
       << "offset=" << offset << std::endl
       << "t=" << t << std::endl;
    io << "started from slope0 = " << slope0
       << " pq0 = " << pq0 << ":" << std::endl;
    const int
        a = std::min(pq0.start, pq.start),
        b = std::max(pq0.stop, pq.stop);
    for (int i = a; i <= b; i++) {
        io << " " << i << ": " << elements[i];
    }
    io << std::endl << "as hex:64:double" << std::endl
       << "x = " << hex(x) << std::endl
       << "slope = " << hex(slope) << std::endl
       << "offset = " << hex(offset) << std::endl
       << "t = " << hex(t) << std::endl;
    dump_events(io, elements, a, b);
}
