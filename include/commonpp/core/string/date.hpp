/*
 * File: include/commonpp/core/string/date.hpp
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

namespace commonpp
{
namespace string
{
std::string get_current_date();
std::string get_date(std::chrono::system_clock::time_point time_point);
} // namespace string
} // namespace commonpp
