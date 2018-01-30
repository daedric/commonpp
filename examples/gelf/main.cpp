/*
 * File: examples/metrics/main.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2018 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/core/LoggingInterface.hpp"
#include "commonpp/thread/ThreadPool.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(int, char *[])
{
    commonpp::core::init_logging();

    commonpp::core::add_gelf_sink(
        "localhost", "12201",
        {{"application_name", "gelf_test"}, {"environment", "production"}});

    commonpp::core::set_logging_level(commonpp::LoggingLevel::trace);

    GLOG(trace)   << "A trace message";
    GLOG(debug)   << "A debug message";
    GLOG(info)    << "An info message";
    GLOG(warning) << "A warning message";
    GLOG(error)   << "An error message";
    GLOG(fatal)   << "A fatal message";

    commonpp::thread::ThreadPool pool(4);
    pool.start([]() 
    {
        for (int i = 0; i < 10000; i++)
        {
            // This message will be sent in chunked format.
            GLOG(warning) << "This really long message should be chunked"
                          << std::string(1000, '!') << ".BYE!";
            GLOG(warning) << "\'this should ☺  be escaped\"";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::minutes(1));
    pool.stop();
    return 0;
}
