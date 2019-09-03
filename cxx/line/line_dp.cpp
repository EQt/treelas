/*
  New implementation
*/
#include <stdexcept>
#include <vector>

#include "../event.hpp"
#include "../range.hpp"
#include "utils/timer.hpp"
#include "bits/weights.hpp"
#include "bits/positive.hpp"


template<int step, typename Tdouble, typename float_ = double>
inline float_
clip(Event *elem,
     Range &pq,
     Tdouble slope,
     float_ offset)
{
    const auto *e = &elem[pq.stop];
}



template<typename float_, typename Wlam, typename Wmu>
void
line_las(
    const size_t n,
    float_ *x,
    const float_ *y,
    const Wlam lam,
    const Wmu mu = ConstantWeights<float_>(float_(1.0)))
{
    if (!is_positive(mu[n-1]))
        throw std::invalid_argument("End node must not be latent");

    std::vector<Event> event_;
    std::vector<float_> ub_;
    Event *event = event_.data();
    float_
        *ub = ub_.data(),
        *lb = x,
        lam0 = float_(0.0);

    for (size_t i = 0; i < n-1; i++) {
        lb[i] = clip<+1>(event, +mu[i], -mu[i] * y[i] - lam0 + lam[i]);
        ub[i] = clip<-1>(event, -mu[i], +mu[i] * y[i] - lam0 + lam[i]);
    }
}
