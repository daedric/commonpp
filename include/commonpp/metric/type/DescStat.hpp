/*
 * File: DescStat.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <boost/accumulators/statistics/weighted_tail_quantile.hpp>
#include <boost/accumulators/statistics/weighted_sum.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_moment.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>

#include <commonpp/metric/detail/types.hpp>
#include <commonpp/metric/MetricValue.hpp>
#include <commonpp/metric/reservoir/types.hpp>

namespace commonpp
{
namespace metric
{
class Metrics;

namespace type
{

// Descriptive statistic.
// It will calculate several features in order to summarize a reservoir.
class DescStat : private boost::noncopyable
{
    using Stats = boost::accumulators::stats<
        boost::accumulators::tag::mean,
        boost::accumulators::tag::min,
        boost::accumulators::tag::max,
        boost::accumulators::tag::count,
        boost::accumulators::tag::variance(boost::accumulators::lazy),
        boost::accumulators::tag::tail_quantile<boost::accumulators::right>>;

    using Accumulator = boost::accumulators::accumulator_set<double, Stats>;

    using WStats = boost::accumulators::stats<
        boost::accumulators::tag::weighted_tail_quantile<boost::accumulators::right>,
        boost::accumulators::tag::weighted_mean,
        boost::accumulators::tag::min,
        boost::accumulators::tag::max,
        boost::accumulators::tag::count,
        boost::accumulators::tag::weighted_variance(boost::accumulators::lazy)>;

    using WAccumulator =
        boost::accumulators::accumulator_set<double, WStats, double>;

public:
    template <typename Reservoir>
    static MetricValue getMetrics(const Reservoir& reservoir)
    {
        return _getMetrics(reservoir, Reservoir::tag);
    }

    template <typename Reservoir>
    static MetricValue _getMetrics(const Reservoir& reservoir,
                                   reservoir::SimpleReservoirTag)
    {
        namespace bacc = boost::accumulators;

        MetricValue result;
        using bacc::mean;
        using bacc::min;
        using bacc::max;
        using bacc::count;
        using bacc::tail_quantile;
        using bacc::variance;

        auto acc = reservoir.template visit<DescStat>();

        if (count(acc) == 0)
        {
            return result;
        }

        result.push(mean(acc), "mean");
        result.push(min(acc), "min");
        result.push(max(acc), "max");
        result.push(variance(acc), "variance");

        result.push(tail_quantile(acc, bacc::quantile_probability = 0.50),
                    "median");
        result.push(tail_quantile(acc, bacc::quantile_probability = 0.75),
                    "p75");
        result.push(tail_quantile(acc, bacc::quantile_probability = 0.95),
                    "p95");
        result.push(tail_quantile(acc, bacc::quantile_probability = 0.99),
                    "p99");

        return result;
    }

    template <typename Reservoir>
    static MetricValue _getMetrics(const Reservoir& reservoir,
                                   reservoir::WeightedReservoirTag)
    {
        namespace bacc = boost::accumulators;

        MetricValue result;
        using bacc::weighted_mean;
        using bacc::min;
        using bacc::max;
        using bacc::count;
        using bacc::weighted_tail_quantile;
        using bacc::weighted_variance;

        WAccumulator acc = reservoir.template visit<DescStat>();

        if (count(acc) == 0)
        {
            return result;
        }

        result.push(weighted_mean(acc), "mean");
        result.push(min(acc), "min");
        result.push(max(acc), "max");
        result.push(weighted_variance(acc), "variance");
        result.push(weighted_tail_quantile(acc, bacc::quantile_probability = 0.50), "median");
        result.push(weighted_tail_quantile(acc, bacc::quantile_probability = 0.75), "p75");
        result.push(weighted_tail_quantile(acc, bacc::quantile_probability = 0.95), "p95");
        result.push(weighted_tail_quantile(acc, bacc::quantile_probability = 0.99), "p99");

        return result;
    }

    struct WeightedProxy
    {
        WeightedProxy(size_t nb_values)
        : acc(boost::accumulators::right_tail_cache_size = nb_values)
        {
        }

        WeightedProxy& operator()(double w, double v)
        {
            acc(v, boost::accumulators::weight = w);
            return *this;
        }

        operator WAccumulator&()
        {
            return acc;
        }

        operator const WAccumulator&() const
        {
            return acc;
        }

        WAccumulator acc;
    };

    static WeightedProxy createAccumulator(size_t nb_values,
                                           reservoir::WeightedReservoirTag)
    {
        return WeightedProxy(nb_values);
    }

    static Accumulator createAccumulator(size_t nb_values,
                                         reservoir::SimpleReservoirTag)
    {
        return Accumulator(
            boost::accumulators::tag::tail<boost::accumulators::right>::cache_size =
                nb_values);
    }
};
} // namespace type
} // namespace metric
} // namespace commonpp
