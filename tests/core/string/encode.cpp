/*
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2017 Thomas Sanchez.  All rights reserved.
 *
 */

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <commonpp/core/string/encode.hpp>

using namespace commonpp::string;

BOOST_AUTO_TEST_CASE(hex_encode_decode)
{
    std::string test("deafbeef");

    BOOST_CHECK_EQUAL(test, hex_encode(hex_decode(test)));
}
