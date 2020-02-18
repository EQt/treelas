cmake_minimum_required(VERSION 3.11)
project(minih5)

set(_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(_MASTER_PROJECT ON)
endif()
option(BUILD_TEST  "Compile unit tests" ${_MASTER_PROJECT})

find_package(HDF5 COMPONENTS C)
if (HDF5_FOUND)
    set(HDF5_LIBRARIES ${HDF5_C_LIBRARIES})

    add_library(minih5 INTERFACE)
    target_include_directories(minih5 INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${HDF5_INCLUDE_DIR})
    target_link_libraries(minih5 INTERFACE ${HDF5_LIBRARIES})
    if (MSVC)
        target_compile_definitions(minih5 INTERFACE H5_BUILT_AS_DYNAMIC_LIB)
    endif()

    if (BUILD_TEST)
        add_subdirectory(test)
    endif()
endif()

# Local Variables:
# cmake-tab-width: 4
# End:
