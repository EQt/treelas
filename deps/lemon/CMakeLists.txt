set(LEMON_ZIP "${CMAKE_CURRENT_SOURCE_DIR}/lemon-1.3.1.tar.gz")
set(LEMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lemon-1-3-r1.3.1")
set(LEMON_URL "http://lemon.cs.elte.hu/hg/lemon-1.3/archive/r1.3.1.tar.gz")

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
    include_directories(${LEMON_DIR})
endif()
