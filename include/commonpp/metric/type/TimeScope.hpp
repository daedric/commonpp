/*
 * File: TimeScope.hpp
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
#include "DescStat.hpp"

namespace commonpp
{
namespace metric
{
namespace type
{

template <typename Reservoir, typename Precision = std::chrono::microseconds>
class TimeScope
{
public:
    TimeScope(Reservoir& r, Clock::time_point start = Clock::now())
    : reservoir_(r)
    , start_(start)
    {
    }

    ~TimeScope()
    {
        if (!drop_)
        {
            auto elapsed = Clock::now() - start_;
            reservoir_.pushValue(
                std::chrono::duration_cast<Precision>(elapsed).count());
        }
    }

    void discard()
    {
        drop_ = true;
    }

private:
    Reservoir& reservoir_;
    const Clock::time_point start_;
    bool drop_ = false;
};

} // namespace type
} // namespace metric
} // namespace commonpp
