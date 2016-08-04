/*
 * File: MetricValue.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/metric/MetricValue.hpp"

#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "commonpp/core/LoggingInterface.hpp"
#include "commonpp/core/string/std_tostring.hpp"
#include "commonpp/core/string/stringify.hpp"
#include "commonpp/metric/detail/types.hpp"

namespace commonpp
{
namespace metric
{

CREATE_LOGGER(mv_logger, "commonpp::metric::MetricValue");

template <>
std::string get_no_metric_marker<std::string>()
{
    return {};
}

static std::string sanitize_influx(const std::string& str)
{
    return boost::replace_all_copy(str, ",", "\\,");
}

static std::string sanitize_graphite(const std::string& str)
{
    return boost::replace_all_copy(str, ".", "_");
}

bool MetricValue::empty() const
{
    return doubles_.empty() && integers_.empty() && booleans_.empty() &&
           strings_.empty();
}

#define ASSERT_NAME                                                            \
    do                                                                         \
    {                                                                          \
        if (name.empty() && has_values())                                      \
        {                                                                      \
            throw std::runtime_error(                                          \
                "A name is required when there are several values");           \
        }                                                                      \
    } while (0)

#define CHECK_NO_VALUE                                                         \
    do                                                                         \
    {                                                                          \
        if (get_no_metric_marker<decltype(value)>() == value)                     \
        {                                                                      \
            return *this;                                                      \
        }                                                                      \
    } while (0)

MetricValue& MetricValue::push(double value, std::string name)
{
    CHECK_NO_VALUE;
    ASSERT_NAME;
    doubles_.emplace_back(move(name), value);
    return *this;
}

MetricValue& MetricValue::push(uintmax_t value, std::string name)
{
    CHECK_NO_VALUE;
    ASSERT_NAME;
    integers_.emplace_back(move(name), value);
    return *this;
}

MetricValue& MetricValue::push(bool value, std::string name)
{
    ASSERT_NAME;
    booleans_.emplace_back(move(name), value);
    return *this;
}

MetricValue& MetricValue::push(std::string value, std::string name)
{
    CHECK_NO_VALUE;
    ASSERT_NAME;
    strings_.emplace_back(move(name), move(value));
    return *this;
}

template <typename Value, typename... Opts>
static const Value&
search(const std::vector<std::pair<std::string, Value>, Opts...>& vector,
       const std::string& name)
{
    for (const auto& p : vector)
    {
        if (p.first == name)
        {
            return p.second;
        }
    }

    throw std::runtime_error("key: \"" + name + "\" not found");
}

double MetricValue::getDouble(const std::string& name) const
{
    return search(doubles_, name);
}

uintmax_t MetricValue::getUint(const std::string& name) const
{
    return search(integers_, name);
}

bool MetricValue::getBool(const std::string& name) const
{
    return search(booleans_, name);
}

const std::string& MetricValue::getString(const std::string& name) const
{
    return search(strings_, name);
}

template <typename T>
static std::ostream& operator<<(std::ostream& os,
                                const std::vector<std::pair<std::string, T>>& v)
{
    for (const auto& e : v)
    {
        os << e.first << ": " << e.second << ", ";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const MetricValue& value)
{
    if (not value.doubles_.empty())
    {
        os << "doubles: {" << value.doubles_ << "}, ";
    }
    if (not value.integers_.empty())
    {
        os << "integers: {" << value.integers_ << "}, ";
    }
    if (not value.booleans_.empty())
    {
        os << "booleans: {" << value.booleans_ << "}, ";
    }
    if (not value.strings_.empty())
    {
        os << "strings: {" << value.strings_ << "}, ";
    }

    return os;
}

size_t MetricValue::metrics() const noexcept
{
    return doubles_.size() + integers_.size() + strings_.size() +
           booleans_.size();
}

using PairSSVector = std::vector<std::pair<std::string, std::string>>;

template <typename T>
std::string MetricValue::to_graphite(const MetricTag& tag,
                                     const T& vector) const
{
    std::string result;

    using Precision = std::chrono::seconds;
    const auto name = tag.toGraphiteFormat();
    const auto timestamp =
        " " + string::stringify(std::chrono::duration_cast<Precision>(
                  capture_time_.time_since_epoch()));

    for (const auto& d : vector)
    {
        result += name + (d.first.empty() ? "" : "." + d.first) + " " +
                  string::stringify(d.second) + timestamp + "\n";
    }

    return result;
}

template <>
std::string MetricValue::to_graphite<PairSSVector>(const MetricTag& tag,
                                                   const PairSSVector& vector) const
{
    std::string result;

    using Precision = std::chrono::seconds;
    const auto name = tag.toGraphiteFormat();
    const auto timestamp =
        " " + string::stringify(std::chrono::duration_cast<Precision>(
                                    capture_time_.time_since_epoch())
                                    .count());

    for (const auto& d : vector)
    {
        result += name +
                  (d.first.empty() ? "" : "." + sanitize_graphite(d.first)) +
                  " " + sanitize_graphite(d.second) + timestamp + "\n";
    }

    return result;
}

template <typename T>
std::string MetricValue::to_influx(const MetricTag&, const T& vector) const
{
    std::string result;

    for (const auto& d : vector)
    {
        result += result.empty() ? "" : ",";
        if (d.first.empty())
        {
            if (metrics() > 1)
            {
                throw std::runtime_error("value name is required for influxdb");
            }

            result += "value";
        }
        else
        {
            result += sanitize_influx(d.first);
        }

        result += "=" + string::stringify(d.second);
    }

    return result;
}

using PairSBVector = std::vector<std::pair<std::string, bool>>;

template <>
std::string MetricValue::to_influx<PairSBVector>(const MetricTag&,
                                                 const PairSBVector& vector) const
{
    std::string result;


    for (const auto& d : vector)
    {
        result += result.empty() ? "" : ",";
        if (d.first.empty())
        {
            if (metrics() > 1)
            {
                throw std::runtime_error("value name is required for influxdb");
            }

            result += "value";
        }
        else
        {
            result += sanitize_influx(d.first);
        }

        result += std::string("=") + (d.second ? "t" : "f");
    }

    return result;
}

std::string MetricValue::toGraphiteFormat(const MetricTag& tag,
                                          const MetricTag& prefix) const
{
    std::string result;

    if (not strings_.empty())
    {
        LOG(mv_logger, warning)
            << "Strings metrics cannot be represented in graphite format.";
    }

    if (!has_values())
    {
        return result;
    }

    auto tag_with_prefix = prefix.hasTags() ? prefix + tag : tag;
    result = to_graphite(tag_with_prefix, doubles_);
    result += to_graphite(tag_with_prefix, integers_);
    result += to_graphite(tag_with_prefix, booleans_);

    return result;
}

std::string MetricValue::toInfluxFormat(const MetricTag& tag,
                                        const MetricTag& prefix) const
{
    std::string result;

    if (!has_values())
    {
        return result;
    }

    auto tag_with_prefix = prefix.hasTags() ? prefix + tag : tag;
    result = tag_with_prefix.toInfluxFormat() + " ";
    result += to_influx(tag_with_prefix, doubles_);
    result += to_influx(tag_with_prefix, integers_);
    result += to_influx(tag_with_prefix, booleans_);
    result += to_influx(tag_with_prefix, strings_);

    using Precision = std::chrono::nanoseconds;
    result += " " + string::stringify(std::chrono::duration_cast<Precision>(
                                          capture_time_.time_since_epoch())
                                          .count()) +
              "\n";

    return result;
}

} // namespace metric
} // namespace commonpp
