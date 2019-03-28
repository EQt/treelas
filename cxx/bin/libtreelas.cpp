#include "utils/timer.hpp"


void
inline _timer_disable()
{
    Timer::set_disable(true);
}


extern "C"
void
timer_disable()
{
    _timer_disable();
}
