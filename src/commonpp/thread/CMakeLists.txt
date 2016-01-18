#
# File: src/commonpp/thread/CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#

add_subdirectory(detail)

add_commonpp_library_source(
    SOURCES
        Thread.cpp
        ThreadPool.cpp)
