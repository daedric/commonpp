#
# File: CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.

include(GenerateExportHeader)

set(ALL_SRCS)

if (${HAVE_SYSLOG})
    add_definitions("-DBOOST_LOG_USE_NATIVE_SYSLOG=1")
endif()

macro(add_commonpp_library_source)
    set(current_srcs)
    foreach(ARG ${ARGN})
        if ("${ARG}" STREQUAL "SOURCES")
            # nothing
        else()
            get_filename_component(file_path "${ARG}" ABSOLUTE)
            list(APPEND current_srcs ${file_path})
        endif()
    endforeach()
    set(ALL_SRCS "${ALL_SRCS};${current_srcs}" PARENT_SCOPE)
endmacro()

add_subdirectory(net)
add_subdirectory(thread)
add_subdirectory(core)

add_library(commonpp ${ALL_SRCS})
target_link_libraries(commonpp ${COMMONPP_DEPS})
generate_export_header(commonpp)
set_property(TARGET commonpp PROPERTY VERSION ${commonpp_VERSION})
set_property(TARGET commonpp PROPERTY SOVERSION ${commonpp_MAJOR})

