/*
 * File: tests/core/options.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
 *
 */
#include <commonpp/core/Options.hpp>

#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace commonpp;

enum class Test : unsigned int
{
    O1 = (1 << 0),
    O2 = (1 << 1),
    O3 = (1 << 2),
    O4 = (1 << 3),
    O5 = (1 << 4),
    O6 = (1 << 5),
};

using TestOS = Options<Test>;
COMMONPP_GENERATE_OPTIONS_OSTREAM_OP(
    TestOS, Test::O1, Test::O2, Test::O3, Test::O4, Test::O5, Test::O6)

BOOST_AUTO_TEST_CASE(basic)
{
    using Opt1 = Options<Test, Test::O1, Test::O2, Test::O3>;
    {
        Opt1 o(Test::O4);
        BOOST_CHECK(o & Test::O1);
        BOOST_CHECK(o & Test::O2);
        BOOST_CHECK(o & Test::O3);
        BOOST_CHECK(o & Test::O4);
        BOOST_CHECK(!(o & Test::O5));
        BOOST_CHECK(!(o & Test::O6));
    }

    using Opt2 = Options<Test>;
    {
        Opt2 o;
        BOOST_CHECK(!(o & Test::O1));
        BOOST_CHECK(!(o & Test::O2));
        BOOST_CHECK(!(o & Test::O3));
        BOOST_CHECK(!(o & Test::O4));
        BOOST_CHECK(!(o & Test::O5));
        BOOST_CHECK(!(o & Test::O6));
    }

    {
        Opt2 o;

        o &= Test::O1;
        BOOST_CHECK(o & Test::O1);

        o += Test::O2;
        BOOST_CHECK(o & Test::O2);

        o -= Test::O2;
        BOOST_CHECK(o & Test::O1);
        BOOST_CHECK(!(o & Test::O2));
    }

    std::cout << TestOS(Test::O1, Test::O6) << std::endl;
}
