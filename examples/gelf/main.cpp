/*
 * File: examples/metrics/main.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/core/LoggingInterface.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(int, char *[])
{

    // Add a graylog GELF sink to the server on localhost and set some
    // static fields that are sent with each log message.
    commonpp::core::add_gelf_sink(
        "localhost", 12201,
        {{"application_name", "gelf_test"}, {"environment", "production"}});

    while (1)
    {
        // This message will be sent in chunked format.
        GLOG(warning) << "This really long message should be chunked"
                      << std::string(1000, '!') << ".BYE!";
        GLOG(warning) << "\'this should ☺ be escaped\"";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
