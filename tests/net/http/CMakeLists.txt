#
# File: tests/net/http/CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#
set(MODULE "net_http")
add_definitions("-DBOOST_TEST_MODULE=${MODULE}")
ADD_COMMONPP_TEST(encode-decode)
ADD_COMMONPP_TEST(request)
ADD_COMMONPP_TEST(response)
