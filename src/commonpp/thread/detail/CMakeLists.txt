#
# File: src/commonpp/thread/detail/CMakeLists.txt
# Part of commonp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
#


if (NOT HWLOC_FOUND)
    return ()
endif()

add_commonpp_library_source(
    SOURCES
        Core.cpp
        Cores.cpp
        logger.cpp)
