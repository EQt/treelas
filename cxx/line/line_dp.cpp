/*
  New implementation
*/
#include <stdexcept>
#include <vector>

#include "../event.hpp"
#include "utils/timer.hpp"
#include "bits/weights.hpp"
#include "bits/positive.hpp"


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
        *lb = x;
}
