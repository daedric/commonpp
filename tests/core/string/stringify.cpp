/*
 * File: tests/core/string/stringify.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <commonpp/core/string/stringify.hpp>

using namespace commonpp::string;

struct T{};
std::ostream& operator<<(std::ostream& os, T const&)
{
    return os << "T";
}

struct T2
{
};

std::string to_string(const T2&)
{
    return "T2";
}

struct T3
{
    std::string to_string() const
    {
        return "T3";
    }
};

BOOST_AUTO_TEST_CASE(basic)
{
    BOOST_CHECK_EQUAL("123", stringify(123));
    BOOST_CHECK_EQUAL("T", stringify(T()));
    BOOST_CHECK_EQUAL("T2", stringify(T2()));
    BOOST_CHECK_EQUAL("T3", stringify(T3()));
}
