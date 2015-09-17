/*
 * File: types.hpp
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
#include <limits>

namespace commonpp
{
namespace metric
{

template <typename T>
static T get_no_metric_marker()
{
    return std::numeric_limits<T>::max();
}

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

} // namespace metric
} // namespace commonpp
