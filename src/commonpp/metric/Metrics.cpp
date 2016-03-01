/*
 * File: Metrics.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/metric/Metrics.hpp"
#include "commonpp/core/Utils.hpp"

namespace commonpp
{
namespace metric
{

Metrics::Metrics(thread::ThreadPool& pool, std::chrono::seconds period)
: period_(period)
, pool_(pool)
{
    schedule_timer();
}

Metrics::~Metrics()
{
    stop();
}

void Metrics::stop()
{
    timer_handle_->cancel();
    std::lock_guard<std::mutex> lock(counters_lock_);
    callbacks_.disconnect_all_slots();
    counters_.clear();
}

void Metrics::add(MetricTag metric_tag, SharedCounter& counter)
{
    auto counter_ptr = std::make_shared<Metrics::Counter>(
        std::bind(&SharedCounter::sum, std::ref(counter)));

    std::lock_guard<std::mutex> lock(counters_lock_);
    counters_.emplace_back(std::move(metric_tag),
                           std::bind(&Counter::getMetrics, counter_ptr));
}

void Metrics::add(MetricTag metric_tag, Gauge gauge)
{
    auto gauge_ptr = std::make_shared<Metrics::Gauge>(std::move(gauge));
    std::lock_guard<std::mutex> lock(counters_lock_);
    counters_.emplace_back(std::move(metric_tag), std::bind(&Gauge::getMetrics, gauge_ptr));
}

void Metrics::add(MetricTag metric_tag, Counter counter)
{
    auto counter_ptr = std::make_shared<Metrics::Counter>(std::move(counter));
    std::lock_guard<std::mutex> lock(counters_lock_);
    counters_.emplace_back(std::move(metric_tag),
                           std::bind(&Counter::getMetrics, counter_ptr));
}

void Metrics::schedule_timer()
{
    timer_handle_ =
        pool_.schedule(period_, std::bind(&Metrics::calculate_metrics, this));
}

void Metrics::removeAll(const MetricTag& prefix)
{
    std::lock_guard<std::mutex> lock(counters_lock_);
    counters_.erase(std::remove_if(counters_.begin(), counters_.end(),
                                   [&prefix](const Generator& generator) {
                                       return prefix.isPrefixOf(generator.first);
                                   }),
                    counters_.end());
}

void Metrics::calculate_metrics()
{
    std::lock_guard<std::mutex> lock(counters_lock_);

    MetricVector metrics;

    for (const auto& counter : counters_)
    {
        auto values = counter.second();
        if (not values.empty())
        {
            metrics.emplace_back(std::cref(counter.first), std::move(values));
        }
    }

    if (!metrics.empty())
    {
        callbacks_(metrics);
    }
}

} // namespace metric
} // namespace commonpp
