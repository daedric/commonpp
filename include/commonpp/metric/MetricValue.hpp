/*
 * File: MetricValue.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "detail/types.hpp"
#include "MetricTag.hpp"

namespace commonpp
{
namespace metric
{

class MetricValue
{
public:
    MetricValue& push(double value, std::string name = "");
    MetricValue& push(uintmax_t value, std::string name = "");
    MetricValue& push(bool value, std::string name = "");
    MetricValue& push(std::string value, std::string name = "");

    std::string toGraphiteFormat(const MetricTag&,
                                 const MetricTag& prefix = {}) const;
    std::string toInfluxFormat(const MetricTag&,
                               const MetricTag& prefix = {}) const;

    friend std::ostream& operator<<(std::ostream&, const MetricValue& tag);

    TimePoint getCapturetime() const
    {
        return capture_time_;
    }

    bool empty() const;

    double getDouble(const std::string& name = "") const;
    uintmax_t getUint(const std::string& name = "") const;
    bool getBool(const std::string& name = "") const;
    const std::string& getString(const std::string& name = "") const;

    size_t metrics() const noexcept;

private:
    bool has_values() const
    {
        return !empty();
    }

    template <typename T>
    std::string to_graphite(MetricTag const&, const T& vector) const;

    template <typename T>
    std::string to_influx(MetricTag const&, const T& vector) const;

private:
    TimePoint capture_time_ = Clock::now();

    std::vector<std::pair<std::string, double>> doubles_;
    std::vector<std::pair<std::string, uintmax_t>> integers_;
    std::vector<std::pair<std::string, bool>> booleans_;
    std::vector<std::pair<std::string, std::string>> strings_;
};

} // namespace metric
} // namespace commonpp
