/*
 * File: tests/metrics/reservoir/edr.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include <iostream>
#include <boost/test/unit_test.hpp>

#include <commonpp/metric/reservoir/ExponentiallyDecayingReservoir.hpp>
#include <commonpp/metric/type/DescStat.hpp>

namespace reservoir = commonpp::metric::reservoir;

// Direct translation of the tests there:
// https://github.com/dropwizard/metrics/blob/master/metrics-core/src/test/java/io/dropwizard/metrics/ExponentiallyDecayingReservoirTest.java
struct Clock : std::chrono::system_clock
{
    using Base = std::chrono::system_clock;

    static Base::time_point now()
    {
        return Base::time_point(
            std::chrono::duration_cast<Base::time_point::duration>(now_));
    }

    static std::chrono::milliseconds now_;
};

std::chrono::milliseconds Clock::now_{0};

struct ResetClock
{
    ~ResetClock()
    {
        Clock::now_ = std::chrono::milliseconds::zero();
    }
};

template <typename R>
static std::vector<std::pair<double, double>> extract_values_and_weight(R& r)
{
    struct T
    {
        std::vector<std::pair<double, double>> vector;
        void operator()(double w, double v)
        {
            vector.emplace_back(w, v);
        }

        static T createAccumulator(size_t, reservoir::WeightedReservoirTag)
        {
            return T();
        }
    };

    return r.template visit<T>().vector;
}

template <typename R>
static std::vector<double> extract_values(R& r)
{
    struct T
    {
        std::vector<double> vector;
        void operator()(double w, double v)
        {
            vector.emplace_back(v);
        }

        static T createAccumulator(size_t, reservoir::WeightedReservoirTag)
        {
            return T();
        }
    };

    return r.template visit<T>().vector;
}

template <size_t s, size_t a>
using ExponentiallyDecayingReservoir =
    commonpp::metric::reservoir::ExponentiallyDecayingReservoir<s, a, Clock>;

using commonpp::metric::type::DescStat;

BOOST_AUTO_TEST_CASE(reservoir_1000_out_of_1000)
{
    ResetClock raii_reset;

    ::commonpp::metric::reservoir::ExponentiallyDecayingReservoir<
        100, 99, std::chrono::system_clock> reservoir;

    for (int i = 0; i < 1000; ++i)
    {
        reservoir.pushValue(i);
    }

    auto values = extract_values(reservoir);
    BOOST_CHECK_EQUAL(values.size(), 100);
    for (auto v : values)
    {
        BOOST_CHECK(v >= 0 && v <= 1000);
    }
}

BOOST_AUTO_TEST_CASE(reservoir_1000_out_of_10)
{
    ResetClock raii_reset;

    ::commonpp::metric::reservoir::ExponentiallyDecayingReservoir<
        100, 99, std::chrono::system_clock> reservoir;

    for (int i = 0; i < 10; ++i)
    {
        reservoir.pushValue(i);
    }

    auto values = extract_values(reservoir);
    BOOST_CHECK_EQUAL(values.size(), 10);
    for (auto v : values)
    {
        BOOST_CHECK(v >= 0 && v <= 10);
    }
}

BOOST_AUTO_TEST_CASE(heavily_biased_reservoir_1000_out_of_1000)
{
    ResetClock raii_reset;

    ::commonpp::metric::reservoir::ExponentiallyDecayingReservoir<
        1000, 1, std::chrono::system_clock> reservoir;

    for (int i = 0; i < 100; ++i)
    {
        reservoir.pushValue(i);
    }

    auto values = extract_values(reservoir);
    BOOST_CHECK_EQUAL(values.size(), 100);
    for (auto v : values)
    {
        BOOST_CHECK(v >= 0 && v <= 100);
    }
}

BOOST_AUTO_TEST_CASE(long_period_of_inactivity_should_not_corrupt_sampling_rate)
{
    ResetClock raii_reset;

    ExponentiallyDecayingReservoir<10, 15> reservoir;

    for (int i = 0; i < 1000; ++i)
    {
        reservoir.pushValue(1000 + i, Clock::now());
        Clock::now_ += std::chrono::milliseconds(100);
    }

    {
        auto values = extract_values(reservoir);
        BOOST_CHECK_EQUAL(values.size(), 10);
        for (auto v : values)
        {
            BOOST_CHECK(v >= 1000 && v <= 2000);
        }
    }

    Clock::now_ += std::chrono::hours(15);
    reservoir.pushValue(2000);
    {
        auto values = extract_values(reservoir);
        BOOST_CHECK_EQUAL(values.size(), 2);
        for (auto v : values)
        {
            BOOST_CHECK(v >= 1000 && v <= 3000);
        }
    }

    for (int i = 0; i < 1000; ++i)
    {
        reservoir.pushValue(3000 + i);
        Clock::now_ += std::chrono::milliseconds(100);
    }

    {
        auto values = extract_values(reservoir);
        BOOST_CHECK_EQUAL(values.size(), 10);
        for (auto v : values)
        {
            BOOST_CHECK(v >= 3000 && v <= 4000);
        }
    }
}

BOOST_AUTO_TEST_CASE(spot_lift)
{
    ResetClock raii_reset;
    ExponentiallyDecayingReservoir<1000, 15> reservoir;

    const int rate_per_min = 10;

    std::chrono::milliseconds interval = std::chrono::minutes(1);
    interval /= 10;

    for (int i = 0; i < 120 * rate_per_min; ++i)
    {
        reservoir.pushValue(177);
        Clock::now_ += interval;
    }

    for (int i = 0; i < 10 * rate_per_min; ++i)
    {
        reservoir.pushValue(9999);
        Clock::now_ += interval;
    }

    auto metrics = DescStat::getMetrics(reservoir);
    BOOST_CHECK_EQUAL(metrics.getDouble("median"), 9999);
}

BOOST_AUTO_TEST_CASE(spot_fall)
{
    ResetClock raii_reset;
    ExponentiallyDecayingReservoir<1000, 15> reservoir;

    const int rate_per_min = 10;

    std::chrono::milliseconds interval = std::chrono::minutes(1);
    interval /= 10;

    for (int i = 0; i < 120 * rate_per_min; ++i)
    {
        reservoir.pushValue(9998);
        Clock::now_ += interval;
    }

    for (int i = 0; i < 10 * rate_per_min; ++i)
    {
        reservoir.pushValue(178);
        Clock::now_ += interval;
    }

    auto metrics = DescStat::getMetrics(reservoir);
    BOOST_CHECK_EQUAL(metrics.getDouble("p95"), 178);
}

BOOST_AUTO_TEST_CASE(quantilies_should_be_based_on_weights)
{
    ResetClock raii_reset;
    ExponentiallyDecayingReservoir<1000, 15> reservoir;

    for (int i = 0; i < 40; ++i)
    {
        reservoir.pushValue(177);
    }

    Clock::now_ += std::chrono::seconds(120);

    for (int i = 0; i < 10; ++i)
    {
        reservoir.pushValue(9999);
    }

    auto values = extract_values_and_weight(reservoir);

    BOOST_CHECK_EQUAL(reservoir.size(), 50);

    auto metrics = DescStat::getMetrics(reservoir);

    BOOST_CHECK_EQUAL(metrics.getDouble("median"), 9999);
    BOOST_CHECK_EQUAL(metrics.getDouble("p75"), 9999);
}
