#
# File: tests/core/string/CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#

set(MODULE "core_string")
add_definitions("-DBOOST_TEST_MODULE=${MODULE}")
ADD_COMMONPP_TEST(stringify)
