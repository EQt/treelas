cmake_minimum_required(VERSION 3.11)
project(minih5)

set(_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(_MASTER_PROJECT ON)
endif()
option(BUILD_TEST  "Compile unit tests" ${_MASTER_PROJECT})

find_package(HDF5 COMPONENTS C)
if (HDF5_FOUND)
    add_library(minih5 INTERFACE)
    set(HDF5_LIBRARIES ${HDF5_C_LIBRARIES})
    target_include_directories(minih5 INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${HDF5_INCLUDE_DIR})
    target_link_libraries(minih5 INTERFACE ${HDF5_LIBRARIES})
    if (MSVC)
        target_compile_definitions(minih5 INTERFACE H5_BUILT_AS_DYNAMIC_LIB)
    endif()

    function (add_executable_h5 name)
        list(REMOVE_AT ARGV 0)
        add_executable(${name} ${ARGV})
        target_link_libraries(${name} minih5)
    endfunction()

    if (BUILD_TEST)
        add_subdirectory(test)
    endif()
endif()

# Local Variables:
# cmake-tab-width: 4
# End:
