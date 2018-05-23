#
# File: CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#
set(commonpp_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/commonpp/core/config.hpp.in")
set(commonpp_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/commonpp/core")
set(commonpp_CONFIG_GENERATED "${commonpp_CONFIG_DIR}/config.hpp")

CONFIGURE_FILE(${commonpp_CONFIG_FILE} ${commonpp_CONFIG_GENERATED})

file(GLOB_RECURSE INCS "*.hpp")
source_group(headers FILES ${INCS})

target_include_directories(commonpp PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${CMAKE_CURRENT_BINARY_DIR}/"
)

