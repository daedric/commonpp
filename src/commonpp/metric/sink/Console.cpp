/*
 * File: /home/thomas/perso/commonpp/src/commonpp/metric/sink/Console.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/metric/sink/Console.hpp"

namespace commonpp
{
namespace metric
{
namespace sink
{

void Console::operator()(const Metrics::MetricVector& values)
{
    for (const auto& m : values)
    {
        const auto& tag = std::get<0>(m).get();
        auto& value = std::get<1>(m);

        std::cout << tag << ": [" << value << "]" << std::endl;
    }
}

} // namespace sink
} // namespace metric
} // namespace commonpp
