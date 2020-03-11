#include <graphidx/utils/timer.hpp>
#include "../tree_dp.hpp"

#ifdef _WIN32
#  define __export __declspec(dllexport)
#elif defined(__GNUC__)
#  define __export __attribute__ ((visibility ("default")))
#else
#  define __export
#endif


void
inline _timer_disable()
{
    Timer::set_disable(true);
}


extern "C" __export void
timer_disable()
{
    _timer_disable();
}


extern "C" __export const double*
tree_dp_f64_i32(
    const size_t n,
    double *x,
    const double *y,
    const int *parent,
    const double lam,
    const double mu,
    int root)
{
    return tree_dp<false, true>(n, x, y, parent, lam, mu, root);
}
