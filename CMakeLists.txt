cmake_minimum_required(VERSION 3.1)
project(treelas)

set(CMAKE_CXX_STANDARD 11)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_definitions(-Wall -g -pedantic -O3)
  add_definitions(-mtune=native)
endif()

option(BUILD_TESTS "Build unit tests using googletest" ON)
option(BUILD_PYEXT "Build python extension module"     ON)

set(PYBIND_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11"
    CACHE STRING "Directory where pybind is located")
set(PYBIND_CMAKE "${PYBIND_DIR}/tools"
    CACHE STRING "Directory where to find pybind11 cmake's tools")
message("-- pybind cmake ${PYBIND_CMAKE}")


add_subdirectory(extern/glmgen)

if (BUILD_PYEXT)
  # download as submodule?
  # if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11/CMakeLists.txt")
  #   execute_process(COMMAND git submodule update --init extern/pybind11
  #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  # endif()
  if (EXISTS "${PYBIND_DIR}/CMakeLists.txt")
    add_subdirectory(${PYBIND_DIR})
  else()
    list(APPEND CMAKE_MODULE_PATH "${PYBIND_CMAKE}")
    include(pybind11Config)
    include(pybind11Tools)
  endif()

  pybind11_add_module(_treelas python/_treelas.cpp)
endif()

configure_file(cxx/config.hpp.in config.hpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(argparser OBJECT cxx/utils/argparser.cpp)

# add_library(dp_line OBJECT cxx/dp_line.cpp)
# set_property(TARGET dp_line PROPERTY POSITION_INDEPENDENT_CODE ON)


find_package(HDF5 COMPONENTS C HL)
set(HDF5_LIBRARIES ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES})


if (BUILD_TESTS)
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init extern/googletest
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  set(BUILD_GMOCK OFF)
  set(BUILD_GTEST ON)
  add_subdirectory(extern/googletest)
  include_directories(extern/googletest/googletest/include)
  set(TESTS
  )
  if (HDF5_FOUND)
    list(APPEND TESTS test/test_hdf5)
  endif()
  add_executable(tests test/tests.cpp ${TESTS})
  target_compile_definitions(tests PUBLIC _STACK_DEBUG=1)
  target_link_libraries(tests gtest_main "${CMAKE_THREAD_LIBS_INIT}")
  if (HDF5_FOUND)
    target_include_directories(tests PUBLIC ${HDF5_INCLUDE_DIR})
    target_link_libraries(tests ${HDF5_LIBRARIES})
  endif()
  enable_testing()
  add_test(NAME cxxtest COMMAND tests)
endif()
