/*
 * File: Gauge.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <functional>
#include <utility>

#include <commonpp/metric/MetricValue.hpp>
#include <commonpp/metric/detail/types.hpp>

namespace commonpp
{
namespace metric
{
namespace type
{

template <typename Type = double>
struct Gauge
{
    template <typename Callable>
    Gauge(Callable fn, std::string name = "")
    : value_(fn)
    , name_(move(name))
    {
    }

    MetricValue getMetrics()
    {
        MetricValue value;
        last_time_point = value.getCapturetime();
        value.push(value_(), name_);
        return value;
    }

    std::function<Type()> value_;
    TimePoint last_time_point;
    std::string name_;
};

} // namespace type
} // namespace metric
} // namespace commonpp
