cmake_minimum_required(VERSION 3.12)
project(treelas)

option(BUILD_TESTS "Build unit tests using googletest" ON)
option(BUILD_PYEXT "Build python extension module"     ON)
option(ASAN        "Use Address SANitizer"             OFF)

# FLAGS
set(CMAKE_CXX_STANDARD 14)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_definitions(-Wall -Wno-error=conversion -Wextra -Wconversion)
  add_definitions(-pedantic -ansi -g)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
  if (ASAN)
    message("-- Enable Address Sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mtune=native -march=native")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mlzcnt")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-Wno-sign-conversion)
  endif()
elseif (MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
if (APPLE)
  set(HDF5_USE_STATIC_LIBRARIES ON)
endif()

include_directories(graphidx/cxx)
include_directories(deps/minih5/include)

add_subdirectory(deps/glmgen)
add_subdirectory(deps/condat)
add_subdirectory(ci)

add_library(argparser OBJECT cxx/utils/argparser.cpp)

add_object(line       cxx/line/line.cpp)
add_object(line_w     cxx/line/line_w.cpp)
add_object(line_para  cxx/line/line_para.cpp)
add_object(line_c     cxx/line/line_c.cpp)
add_object(line_c2    cxx/line/line_c2.cpp)
add_object(line_c3    cxx/line/line_c3.cpp)
add_object(tree_dp    cxx/tree_dp.cpp)
add_object(tree_dual  cxx/tree_dual.cpp)

# PYTHON
set(PYBIND_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/deps/pybind11"
    CACHE STRING "Directory where pybind is located")
if (BUILD_PYEXT)
  # download as submodule?
  # if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/pybind11/CMakeLists.txt")
  #   execute_process(COMMAND git submodule update --init deps/pybind11
  #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  # endif()
  if (NOT EXISTS "${PYBIND_DIR}/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init deps/pybind11
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  add_subdirectory(${PYBIND_DIR})
  pybind11_add_module(_treelas
    python/_treelas.cpp
    python/line.cpp
    python/order.cpp
    python/tree.cpp
    python/idx.cpp
    python/spanning.cpp
    cxx/tree_12x.cpp
    $<TARGET_OBJECTS:condat_tv>
    $<TARGET_OBJECTS:tf_dp>
    $<TARGET_OBJECTS:line>
    $<TARGET_OBJECTS:line_w>
    $<TARGET_OBJECTS:line_para>
    $<TARGET_OBJECTS:line_c>
    $<TARGET_OBJECTS:line_c2>
    $<TARGET_OBJECTS:line_c3>
    $<TARGET_OBJECTS:tree_dp>
    $<TARGET_OBJECTS:tree_dual>
  )
  # target_link_libraries(_treelas PUBLIC -pthread)
  add_custom_target(pysetup  DEPENDS _treelas
    COMMAND ${CMAKE_COMMAND} -E copy
      $<TARGET_FILE_NAME:_treelas> ${CMAKE_SOURCE_DIR}/python/treelas)
  add_custom_target(pytest DEPENDS pysetup
    COMMAND py.test ${CMAKE_SOURCE_DIR}/python/test VERBATIM)
endif()


# HDF5
find_package(HDF5 COMPONENTS C)
if (HDF5_FOUND)
  set(HDF5_LIBRARIES ${HDF5_C_LIBRARIES})
  if (MSVC)
    add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
  endif()

  add_executable_h5(traverse
    cxx/app/traverse.cpp
    $<TARGET_OBJECTS:argparser>
  )

  add_executable_h5(tree_apx
    cxx/app/tree_apx.cpp
    $<TARGET_OBJECTS:argparser>
  )

  add_executable_h5(tree_12x
    cxx/app/tree_12x.cpp
    cxx/tree_12x.cpp
    $<TARGET_OBJECTS:argparser>
  )

  add_executable_h5(tree_solve
    cxx/app/tree_solve.cpp
    $<TARGET_OBJECTS:tree_dp>
    $<TARGET_OBJECTS:argparser>
  )
endif()


# TESTS
if (BUILD_TESTS)
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/googletest/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init deps/googletest
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  set(BUILD_GMOCK OFF)
  set(BUILD_GTEST ON)
  set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")
  add_subdirectory(deps/googletest)
  include_directories(deps/googletest/googletest/include)
  add_subdirectory(graphidx)
  get_directory_property(GRAPHIDX_TESTS DIRECTORY graphidx DEFINITION TESTS)
  list(TRANSFORM GRAPHIDX_TESTS PREPEND "graphidx/cxx/test/")
  set(TESTS
    cxx/test/test_vecalloc.cpp
    cxx/test/test_ref.cpp
    cxx/test/test_strtok.cpp
    cxx/test/test_line.cpp
    cxx/test/test_tree_12.cpp
    cxx/test/test_tree_dp.cpp
  )
  if (HDF5_FOUND)
    list(APPEND TESTS deps/minih5/test/test_hdf5.cpp)
  endif()
  add_executable(tests
    cxx/test/tests.cpp
    ${GRAPHIDX_TESTS}
    ${TESTS}
    $<TARGET_OBJECTS:line>
  )
  target_compile_definitions(tests PUBLIC _STACK_DEBUG=1)
  target_link_libraries(tests gtest_main "${CMAKE_THREAD_LIBS_INIT}")
  if (HDF5_FOUND)
    target_include_directories(tests PUBLIC ${HDF5_INCLUDE_DIR})
    target_link_libraries(tests ${HDF5_LIBRARIES})
  endif()
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_libraries(tests -v)
  endif()
  enable_testing()
  add_test(NAME cxxtest COMMAND tests)
endif()
