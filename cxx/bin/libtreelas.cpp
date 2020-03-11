#include <graphidx/utils/timer.hpp>
#include "../tree_dp.hpp"
#include "../tree_dual.hpp"

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


extern "C" __export double*
tree_dual_f64_i32(
    const size_t n,
    double *x,
    const int *parent,
    const int *postord,
    double *alpha,
    const int root,
    const unsigned char tree_orientation)
{
    return tree_dual(n, x, parent, postord, alpha, root, tree_orientation != 0);
}
