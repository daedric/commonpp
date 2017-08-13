/*
 * File: MetricTag.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/metric/MetricTag.hpp"

#include <iostream>
#include <boost/algorithm/string.hpp>

namespace commonpp
{
namespace metric
{

MetricTag::MetricTag(std::string name)
: name_(move(name))
{
}

MetricTag MetricTag::create(std::string measure_name) const
{
    MetricTag tmp = *this;
    if (!tmp.name_.empty())
    {
        tmp.name_ += "." + measure_name;
    }
    else
    {
        tmp.name_ = move(measure_name);
    }
    return tmp;
}

MetricTag MetricTag::operator()(std::string name) const
{
    return create(name);
}

bool MetricTag::hasTags() const noexcept
{
    return !tags_.empty();
}

MetricTag& MetricTag::operator+=(std::initializer_list<std::string>&& t)
{
    if (t.size() != 2)
    {
        throw std::runtime_error("initializer_list is supposed to be {k, v}");
    }

    return pushTag(std::move(*t.begin()), std::move(*(t.begin() + 1)));
}

static std::string sanitize_influx(const std::string& str)
{
    return boost::replace_all_copy(str, ",", "\\,");
}

static std::string sanitize_graphite(const std::string& str)
{
    return boost::replace_all_copy(str, ".", "_");
}

MetricTag& MetricTag::pushTag(std::string name, std::string value)
{
    tags_.emplace_back(move(name), move(value));
    return *this;
}

std::ostream& operator<<(std::ostream& os, const MetricTag& tag)
{
    os << "measure: " << tag.name_ << ", tags: {";
    for (const auto& t : tag.tags_)
    {
        os << t.first << ": " << t.second << ", ";
    }

    return os << "}";
}

MetricTag MetricTag::operator+(const MetricTag& rhs) const
{
    MetricTag result = *this;
    result.tags_.insert(std::end(result.tags_), std::begin(rhs.tags_),
                        std::end(rhs.tags_));
    if (!result.name_.empty())
    {
        result.name_ += "." + rhs.name_;
    }
    else
    {
        result.name_ = rhs.name_;
    }

    return result;
}

const std::string& MetricTag::toInfluxFormat() const
{
    if (!influx_.empty())
    {
        return influx_;
    }

    if (name_.empty())
    {
        throw std::runtime_error("a name must be given to the MetricTag");
    }

    influx_ = sanitize_influx(name_);

    for (const auto& tag : tags_)
    {
        influx_ += ",";
        influx_ += sanitize_influx(tag.first) + "=" + sanitize_influx(tag.second);
    }

    return influx_;
}

const std::string& MetricTag::toGraphiteFormat(const std::string& prefix) const
{
    if (!graphite_.empty())
    {
        return graphite_;
    }

    graphite_ = prefix;
    if (!(graphite_.empty() || boost::ends_with(graphite_, ".")))
    {
        graphite_ += ".";
    }

    for (const auto& tag : tags_)
    {
        graphite_ += sanitize_graphite(tag.second) + ".";
    }

    if (!name_.empty()) // We want to keep the dots
    {
        graphite_ += name_;
    }

    return graphite_;
}

bool MetricTag::isPrefixOf(const MetricTag& tag) const
{
    if (tags_.size() > tag.tags_.size())
    {
        return false;
    }

    return std::equal(tags_.begin(), tags_.end(), tag.tags_.begin()) &&
           (name_.empty() || boost::starts_with(tag.name_, name_));
}

} // namespace metric
} // namespace commonpp
