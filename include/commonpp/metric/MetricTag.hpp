/*
 * File: MetricTag.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <initializer_list>
#include <iosfwd>
#include <string>
#include <vector>

namespace commonpp
{
namespace metric
{

class MetricTag
{
public:
    MetricTag(std::string name = "");
    ~MetricTag() = default;

    const std::string& toInfluxFormat() const;
    // For the graphite format, all the tag values will be catenated using '.'
    // in the order they were added.
    const std::string& toGraphiteFormat(const std::string& prefix = "") const;

    bool hasTags() const noexcept;

    MetricTag& pushTag(std::string name, std::string value);
    MetricTag& operator+=(std::initializer_list<std::string>&&);
    MetricTag operator+(const MetricTag&) const;

    friend std::ostream& operator<<(std::ostream&, const MetricTag& tag);

    // Create a new MetricTag based on the current one but with the given name.
    MetricTag create(std::string measure_name) const;
    MetricTag operator()(std::string measure_name) const;

    bool isPrefixOf(const MetricTag& tag) const;

private:
    std::string name_;
    std::vector<std::pair<std::string, std::string>> tags_;

    mutable std::string influx_;
    mutable std::string graphite_;
};

} // namespace metric
} // namespace commonpp
