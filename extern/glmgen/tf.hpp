#pragma once
#define HAVE_GLMGEN 1
#define HAVE_TIMER  1

#ifdef HAVE_TIMER
#include "../../cxx/utils/timer.hpp"
#endif

#ifdef HAVE_GLMGEN
#  include <cstdlib>

#  ifdef __cplusplus
namespace glmgen {
extern "C" {
#  endif

#  include "tf.h"

#ifdef __cplusplus
}
}
#endif
#endif
