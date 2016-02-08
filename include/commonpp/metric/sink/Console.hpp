/*
 * File: include/commonpp/metric/sink/Console.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <commonpp/metric/Metrics.hpp>

namespace commonpp
{
namespace metric
{
namespace sink
{

class Console
{
public:
    Console() = default;
    ~Console() = default;

    void operator()(const Metrics::MetricVector& values);
};

} // namespace sink
} // namespace metric
} // namespace commonpp
