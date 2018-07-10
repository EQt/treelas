cmake_minimum_required(VERSION 3.1)

set(GLMGEN https://raw.githubusercontent.com/statsmaths/glmgen)
set(tf_dpc ${CMAKE_CURRENT_SOURCE_DIR}/tf_dp.c)

if (NOT EXISTS ${tf_dpc})
  file(DOWNLOAD ${GLMGEN}/master/c_lib/glmgen/src/tf/tf_dp.c ${tf_dpc}
    EXPECTED_MD5 "76e29e8c8f5c0f162d69a7d202eef36a")
endif()


if (EXISTS ${tf_dpc})
  add_library(tf_dp OBJECT tf_dp.cpp)
  set_property(TARGET tf_dp PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
