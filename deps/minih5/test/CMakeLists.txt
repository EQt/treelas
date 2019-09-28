include(FetchContent)
FetchContent_Declare(doctesth
    URL https://raw.githubusercontent.com/onqtam/doctest/2.3.5/doctest/doctest.h
    DOWNLOAD_NO_EXTRACT ON
    DOWNLOAD_DIR doctest
    SUBBUILD_DIR .
)
FetchContent_GetProperties(doctesth)
FetchContent_Populate(doctesth)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
add_executable(doctests
    doctests.cpp
    test_hdf5.cpp
)
target_link_libraries(doctests minih5)
