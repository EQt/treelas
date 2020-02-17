cmake_minimum_required(VERSION 3.11)
project(treelas)

option(BUILD_TESTS "Build unit tests using googletest" ON)
option(BUILD_PYEXT "Build python extension module"     ON)
option(FAST_MATH   "Use -ffast-math"                   OFF)
option(ASAN        "Use Address SANitizer"             OFF)
set(GRAPHIDX_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/deps/graphidx"
    CACHE STRING   "Which graphidx to be used")
set(DOCTEST_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/deps/graphidx/deps/doctest"
    CACHE STRING   "Which graphidx to be used")

# FLAGS
set(CMAKE_CXX_STANDARD 14)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_definitions(-Wall -Wno-error=conversion -Wextra -Wconversion)
    add_definitions(-pedantic -ansi -g)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
    if (ASAN)
        message("-- Enable Address Sanitizer")
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mlzcnt")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mtune=native -march=native")
    if (FAST_MATH)
        set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_definitions(-Wno-sign-conversion)
    endif()
endif()

if (NOT EXISTS "${GRAPHIDX_DIR}/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init ${GRAPHIDX_DIR})
endif()

include_directories(deps/graphidx/cxx)
include_directories(deps/argparser)

add_subdirectory(deps/glmgen)
add_subdirectory(deps/condat)
add_subdirectory(deps/graphidx)
add_subdirectory(deps/minih5)
add_subdirectory(ci)

add_object(line_dp    cxx/line_dp.cpp)
add_object(line_para  cxx/line/line_para.cpp)
add_object(line_c     cxx/line/line_c.cpp)
add_object(line_c2    cxx/line/line_c2.cpp)
add_object(line_c3    cxx/line/line_c3.cpp)
add_object(tree_dp    cxx/tree_dp.cpp)
add_object(tree_dual  cxx/tree_dual.cpp)

add_library(argparser OBJECT deps/argparser/argparser.cpp)
add_library(treelas SHARED
    cxx/bin/libtreelas.cpp
    $<TARGET_OBJECTS:tree_dp>
    $<TARGET_OBJECTS:tree_dual>
)

# PYTHON
if (BUILD_PYEXT)
    message("-- Enable python extension")
    pybind11_add_module(_treelas
        python/_treelas.cpp
        python/line.cpp
        python/tree.cpp
        cxx/tree_apx.cpp
        deps/graphidx/python/timer.cpp
        $<TARGET_OBJECTS:condat_tv>
        $<TARGET_OBJECTS:tf_dp>
        $<TARGET_OBJECTS:line_dp>
        $<TARGET_OBJECTS:line_para>
        $<TARGET_OBJECTS:line_c>
        $<TARGET_OBJECTS:line_c2>
        $<TARGET_OBJECTS:line_c3>
        $<TARGET_OBJECTS:tree_dp>
        $<TARGET_OBJECTS:tree_dual>
    )

    add_custom_target(pysetup DEPENDS _treelas graphidx_pysetup
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE_NAME:_treelas> ${CMAKE_SOURCE_DIR}/python/treelas)
    add_custom_target(pytest DEPENDS pysetup
        COMMAND py.test ${CMAKE_SOURCE_DIR}/python/test VERBATIM)
endif()

find_package(HDF5 COMPONENTS C)
if (HDF5_FOUND)
    add_executable_h5(traverse
        cxx/bin/traverse.cpp
        $<TARGET_OBJECTS:argparser>
    )
    add_executable_h5(grid
        cxx/bin/grid.cpp
        $<TARGET_OBJECTS:argparser>
    )
    add_executable_h5(tree_apx
        cxx/bin/tree_apx.cpp
        cxx/tree_apx.cpp
        $<TARGET_OBJECTS:argparser>
    )
    add_executable_h5(tree_solve
        cxx/bin/tree_solve.cpp
        $<TARGET_OBJECTS:tree_dp>
        $<TARGET_OBJECTS:argparser>
    )
endif()


if (BUILD_TESTS)
    message("-- Enable unit tests")
    if (NOT EXISTS "${DOCTEST_DIR}/CMakeLists.txt")
        execute_process(COMMAND git submodule update --init ${DOCTEST_DIR}
            WORKING_DIRECTORY ${GRAPHIDX_DIR})
    endif()
    add_subdirectory(${DOCTEST_DIR})
    add_executable(doctests
        cxx/test/doctests.cpp
        ${TESTS}
        cxx/test/test_line.cpp
        cxx/test/test_strtok.cpp
        cxx/test/test_ref.cpp
        cxx/test/test_clip.cpp
        cxx/test/test_line_dp.cpp
        cxx/test/test_tree_dp.cpp
        cxx/test/test_tree_dp_2.cpp
        cxx/test/test_tree_apx.cpp
    )
    target_link_libraries(doctests PRIVATE doctest::doctest graphidx)
endif()

# Local Variables:
# cmake-tab-width: 4
# End:
