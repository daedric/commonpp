#
# File: bench/CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#

subdirlist(subdirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subdir ${subdirs})
    add_subdirectory(${subdir})
endforeach()

