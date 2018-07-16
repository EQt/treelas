cmake_minimum_required(VERSION 3.1)
project(treelas)

set(CMAKE_CXX_STANDARD 11)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_definitions(-Wall -Wno-error=conversion -Wextra -Wconversion)
  add_definitions(-pedantic -ansi -g)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mtune=native -march=native")
elseif (MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(extern/glmgen)
add_subdirectory(extern/condat)
add_subdirectory(cmake)


add_object(line cxx/line.cpp)


add_library(libtl SHARED
  $<TARGET_OBJECTS:condat_tv>
  $<TARGET_OBJECTS:tf_dp>
  $<TARGET_OBJECTS:line>
)


option(BUILD_TESTS "Build unit tests using googletest" ON)
option(BUILD_PYEXT "Build python extension module"     ON)

set(PYBIND_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11"
    CACHE STRING "Directory where pybind is located")
if (BUILD_PYEXT)
  # download as submodule?
  # if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11/CMakeLists.txt")
  #   execute_process(COMMAND git submodule update --init extern/pybind11
  #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  # endif()
  if (NOT EXISTS "${PYBIND_DIR}/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init extern/pybind11
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  add_subdirectory(${PYBIND_DIR})
  pybind11_add_module(_treelas
    python/_treelas.cpp
    $<TARGET_OBJECTS:condat_tv>
    $<TARGET_OBJECTS:tf_dp>
    $<TARGET_OBJECTS:line>
  )
  add_custom_target(pysetup  DEPENDS _treelas
    COMMAND ${CMAKE_COMMAND} -E copy
      $<TARGET_FILE_NAME:_treelas> ${CMAKE_SOURCE_DIR}/python/treelas)
  add_custom_target(pytest DEPENDS pysetup
    COMMAND py.test ${CMAKE_SOURCE_DIR}/python/test VERBATIM)
endif()


add_library(argparser OBJECT cxx/utils/argparser.cpp)

if (APPLE)
  set(HDF5_USE_STATIC_LIBRARIES ON)
endif()
find_package(HDF5 COMPONENTS C)
if (HDF5_FOUND)
  set(HDF5_LIBRARIES ${HDF5_C_LIBRARIES})
  if (MSVC)
    add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
  endif()
endif()


if (BUILD_TESTS)
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init extern/googletest
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  set(BUILD_GMOCK OFF)
  set(BUILD_GTEST ON)
  set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")
  add_subdirectory(extern/googletest)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(gtest PUBLIC -Wno-conversion)
  endif()
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(gtest PUBLIC -Wno-missing-field-initializers)
  endif()
  include_directories(extern/googletest/googletest/include)
  set(TESTS
    test/test_strtok.cpp
  )
  if (HDF5_FOUND)
    list(APPEND TESTS test/test_hdf5.cpp)
  endif()
  add_executable(tests
    test/tests.cpp
    ${TESTS}
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
