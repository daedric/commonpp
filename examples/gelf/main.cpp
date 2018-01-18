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

int main(int, char *[])
{
    commonpp::core::add_gelf_sink(
        "localhost", 12201,
        {{"application_name", "gelf_test"}, {"environment", "production"}});

    GLOG(warning) << "Really big string " << std::string(1000, '!') << "BYE!";

    GLOG(warning) << "\'this should be escaped\"";
    return 0;
}
