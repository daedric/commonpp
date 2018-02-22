/*
 * File: Utils.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include <chrono>

#include <boost/asio/ip/host_name.hpp>

namespace commonpp
{

std::string get_hostname()
{
    return boost::asio::ip::host_name();
}

std::chrono::seconds get_current_timestamp()
{
    using Clock = std::chrono::system_clock;
    auto now = Clock::now();
    auto time_since_epoch = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
}

} // namespace commonpp
