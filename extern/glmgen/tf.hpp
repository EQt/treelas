#pragma once
#define HAVE_GLMGEN 1

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
