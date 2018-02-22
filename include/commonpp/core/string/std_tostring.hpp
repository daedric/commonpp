/*
 * File: include/commonpp/core/string/std_tostring.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <chrono>
#include <string>

namespace std
{
inline const string& to_string(const string& str)
{
    return str;
}
namespace chrono
{

template <typename Rep, typename Ratio>
std::string to_string(const duration<Rep, Ratio>& duration)
{
    return std::to_string(duration.count());
}

} // namespace chrono
} // namespace std
