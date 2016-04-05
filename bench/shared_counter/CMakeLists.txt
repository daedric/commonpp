#
# File: bench/shared_counter/CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#

add_executable(shared_counter_bench  main.cpp)
target_link_libraries(shared_counter_bench commonpp)

