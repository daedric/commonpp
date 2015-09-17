/*
 * File: string_date.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/core/string/date.hpp"

#include <sstream>

namespace commonpp
{
namespace string
{
std::string get_current_date()
{
    std::time_t current_time = std::time(nullptr);
    std::ostringstream out;

    std::tm tm;
    ::gmtime_r(&current_time, &tm);

    char buff[64] = {0};
    auto size = std::strftime(buff, sizeof(buff), "%c %Z", &tm);

    return {buff, size};
}

std::string get_date(std::chrono::system_clock::time_point time_point)
{
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        time_point.time_since_epoch());

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            time_point.time_since_epoch()) -
                        seconds;

    auto sec = static_cast<time_t>(seconds.count());
    std::tm tm;
    ::gmtime_r(&sec, &tm);

    char buff[32] = {0};
    auto size = std::strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buff, size) + "." + std::to_string(milliseconds.count());
}

} // namespace string
} // namespace commonpp
