/*
 * File: tests/net/http/response.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include <boost/test/unit_test.hpp>
#include <commonpp/net/http/Response.hpp>
#include <string>

using namespace commonpp::net::http;

const std::string RAW = "HTTP/1.1 204 No Content\r\n"
                        "Request-Id: 8ac8f330-5c49-11e5-8005-000000000000\r\n"
                        "X-Influxdb-Version: 0.9.4.1\r\n"
                        "Date: Wed, 16 Sep 2015 04:04:23 GMT\r\n\r\n";

const std::string RAW_2 = "HTTP/1.1 204 No Content\r\n\r\n";
const std::string RAW_3 = "HTTP/1.1 204 No Content\r\n"
                          "Request-Id:\r\n\r\n";

const std::string RAW_INVALID_1 =
    "HTTP/1.1 a204 No Content\r\n"
    "Request-Id: 8ac8f330-5c49-11e5-8005-000000000000\r\n"
    "X-Influxdb-Version: 0.9.4.1\r\n"
    "Date: Wed, 16 Sep 2015 04:04:23 GMT\r\n\r\n";

const std::string RAW_INVALID_2 = "HTTP/1";

const std::string RAW_INVALID_3 = "HTTP/1.1 204 No Content\r\n"
                                  "Request-Id\r\n";

const std::string RAW_INVALID_4 = "HTTP/1.1 204\r\n"
                                  "Request-Id: e\r\n\r\n";

BOOST_AUTO_TEST_CASE(response_parsing)
{
    auto response = Response::from(RAW);

    BOOST_CHECK_EQUAL(response.code(), 204);
    BOOST_CHECK_EQUAL(response.message(), "No Content");
    auto header = response.headers();
    BOOST_CHECK_EQUAL(header["Request-Id"],
                      "8ac8f330-5c49-11e5-8005-000000000000");
    BOOST_CHECK_EQUAL(header["X-Influxdb-Version"], "0.9.4.1");
    BOOST_CHECK_EQUAL(header["Date"], "Wed, 16 Sep 2015 04:04:23 GMT");
}

BOOST_AUTO_TEST_CASE(response_parsing_no_header)
{
    auto response = Response::from(RAW_2);

    BOOST_CHECK_EQUAL(response.code(), 204);
    BOOST_CHECK_EQUAL(response.message(), "No Content");
}

BOOST_AUTO_TEST_CASE(response_parsing_empty_value_in_header)
{
    auto response = Response::from(RAW_3);

    BOOST_CHECK_EQUAL(response.code(), 204);
    BOOST_CHECK_EQUAL(response.message(), "No Content");
    auto header = response.headers();
    BOOST_CHECK_EQUAL(header["Request-Id"], "");
}

BOOST_AUTO_TEST_CASE(invalid)
{
    BOOST_CHECK_THROW(Response::from(RAW_INVALID_1), std::runtime_error);
    BOOST_CHECK_THROW(Response::from(RAW_INVALID_2), std::runtime_error);
    BOOST_CHECK_THROW(Response::from(RAW_INVALID_3), std::runtime_error);
    BOOST_CHECK_THROW(Response::from(RAW_INVALID_4), std::runtime_error);
}
