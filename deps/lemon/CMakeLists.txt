set(LEMON_VERSION "1.3.1")
set(LEMON_ZIP "${CMAKE_CURRENT_SOURCE_DIR}/lemon-${LEMON_VERSION}.tar.gz")
set(LEMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lemon-1-3-r${LEMON_VERSION}")
set(LEMON_URL "http://lemon.cs.elte.hu/hg/lemon-1.3/archive/r${LEMON_VERSION}.tar.gz")

if (WITH_LEMON)
    message("-- With LEMON")
    if (NOT EXISTS ${LEMON_ZIP})
        message("-- Downloading ${LEMON_ZIP}")
        file(DOWNLOAD ${LEMON_URL} ${LEMON_ZIP}
            EXPECTED_HASH MD5=bbdd898b86917ce7886ae05fc244535e)
    endif()
    if (NOT EXISTS ${LEMON_DIR}/lemon/bfs.h)
        if (CMAKE_VERSION VERSION_LESS "3.18")
            execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${LEMON_ZIP}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        else()
            file(ARCHIVE_EXTRACT
                INPUT ${LEMON_ZIP}
                DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
    endif()
    if (NOT EXISTS ${LEMON_DIR}/lemon/config.h)
        set(PROJECT_VERSION ${LEMON_VERSION})
        include(CheckTypeSize)
        check_type_size("long long" LONG_LONG)
        set(LEMON_HAVE_LONG_LONG ${HAVE_LONG_LONG})
        CONFIGURE_FILE(
            ${LEMON_DIR}/lemon/config.h.in
            ${LEMON_DIR}/lemon/config.h
        )
    endif()

    add_library(lemon STATIC ${LEMON_DIR}/lemon/base.cc)
    target_include_directories(lemon PUBLIC ${LEMON_DIR})
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(lemon PUBLIC -Wno-conversion)
    endif()
endif()
