#pragma once
#define HAVE_GLMGEN 1
#define HAVE_TIMER  1


#ifdef HAVE_GLMGEN
#  include <cstdlib>
#  ifdef HAVE_TIMER
#     include <graphidx/utils/timer.hpp>
#  endif
#  ifdef __cplusplus
namespace glmgen {
extern "C" {
#  endif
#  include "tf.h"
#  ifdef __cplusplus
}
}
#  endif
#endif
