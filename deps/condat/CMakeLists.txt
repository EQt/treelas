cmake_minimum_required(VERSION 3.1)

set(CONDAT_WEB https://lcondat.github.io)
set(tvc ${CMAKE_CURRENT_SOURCE_DIR}/condat_tv_v2_.c)

if (NOT EXISTS ${tvc})
  file(DOWNLOAD ${CONDAT_WEB}/download/Condat_TV_1D_v2.c ${tvc}
    EXPECTED_MD5 "2caecd85d35896746fa7dbf5f88262a8")
endif()


if (EXISTS ${tvc})
  add_library(condat_tv OBJECT condat_tv_v2.cpp)
  set_property(TARGET condat_tv PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
