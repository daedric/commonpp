/*
 * File: tests/net/http/request.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include <boost/test/unit_test.hpp>
#include <commonpp/net/http/Request.hpp>

using namespace commonpp::net::http;

const std::string REFERENCE = "GET /help?q=test&t=hello%20world HTTP/1.1\r\n"
                              "toto: 1234\r\n"
                              "Content-Length: 11\r\n"
                              "\r\n"
                              "hello world";

BOOST_AUTO_TEST_CASE(request_building)
{
    Request req;

    req.path("/help");

    req.query() += std::make_pair("q", "test");
    req.query() += std::make_pair("t", "hello world");

    req.headers() += std::make_pair("toto", 1234);

    req.body("hello world");

    auto buffer = req.buildRequest();
    std::string str(buffer.data(), buffer.size());
    BOOST_CHECK_EQUAL(REFERENCE, str);
}
