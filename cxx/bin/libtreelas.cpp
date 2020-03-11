#include <graphidx/utils/timer.hpp>
#include "../tree_dp.hpp"


void
inline _timer_disable()
{
    Timer::set_disable(true);
}


extern "C" void
timer_disable()
{
    _timer_disable();
}


extern "C" const double*
tree_dp_f64_i32(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    int root)
{
    return tree_dp<false, true>(n, x, y, parent, lam, mu);
}
