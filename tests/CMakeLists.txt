#
# File: tests/CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#

add_definitions(-DBOOST_TEST_MAIN)
macro(ADD_COMMONPP_TEST TEST_NAME)
    set(CONFIGURE)
    set(LIBRARY)

    foreach(ARG ${ARGN})
        if (ARG STREQUAL "LIBRARY")
            set(CURRENT LIBRARY)
        elseif(ARG STREQUAL "CONFIGURE_HEADER")
            set(CURRENT CONFIGURE)
        else()
            list(APPEND ${CURRENT} "${ARG}")
        endif()
    endforeach()

    foreach(FILENAME ${CONFIGURE})
        get_filename_component(BASE_NAME ${FILENAME} NAME_WE)
        configure_file(${FILENAME} "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}.hpp" @ONLY)
        include_directories(${CMAKE_CURRENT_BINARY_DIR})
    endforeach()

    set(LIBRARIES ${LIBRARY} commonpp)
    set(EXE_NAME Test_${MODULE}_${TEST_NAME})
    # current project
    include_directories(${CMAKE_SOURCE_DIR}/)
    add_executable(${EXE_NAME} ${TEST_NAME}.cpp)
    target_link_libraries(${EXE_NAME} ${LIBRARIES})
    set(TEST "${MODULE}_${TEST_NAME}")
    add_test(
      NAME
        ${TEST}
      COMMAND
        ${EXE_NAME} ${ARGUMENTS}
    )

    set_tests_properties(${TEST} PROPERTIES TIMEOUT 20)
endmacro()

subdirlist(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subdir ${subdirs})
    add_subdirectory(${subdir})
endforeach()
