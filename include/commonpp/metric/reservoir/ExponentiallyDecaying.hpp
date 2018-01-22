/*
 * File: ExponentiallyDecaying.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <algorithm>
#include <random>
#include <chrono>
#include <mutex>
#include <cmath>

#include <map>

#include <mutex>
#include "types.hpp"

namespace commonpp
{
namespace metric
{
namespace reservoir
{

// Implementation from:
// https://github.com/dropwizard/metrics/blob/master/metrics-core/src/main/java/io/dropwizard/metrics/ExponentiallyDecayingReservoir.java

template <size_t size_ = 1028,
          size_t alpha_time_1000 = 69, // doubles all 10s
          typename clock_ = std::chrono::system_clock,
          typename Mutex = std::mutex>
class ExponentiallyDecaying
{

public:
    static const WeightedReservoirTag tag;

    static constexpr const double ALPHA = double(alpha_time_1000) * 0.001;
    using ElapsedTimeUnit = std::chrono::seconds;

    static constexpr const size_t MAX_SIZE = size_;
    using Clock = clock_;
    using Timepoint = typename Clock::time_point;
    using Weight = double;

    struct Sample
    {
        Weight mc_weight; //!< random weight for sampling multiplied with weight
        Weight weight;    //!< time-based weight for exponential decay
        double value;

        bool operator < (const Sample &o) const
        {
            // for creating a min heap
            return mc_weight > o.mc_weight;
        }
    };

    const std::chrono::minutes RESCALE_INTERVAL{10};

public:
    ExponentiallyDecaying();
    ~ExponentiallyDecaying() = default;

    void pushValue(double value, Timepoint timepoint = Clock::now());

    template <typename Summary>
    auto visit() const -> decltype(Summary::createAccumulator(0, tag))
    {
        std::lock_guard<Mutex> lock(mutex_);
        auto acc = Summary::createAccumulator(size_unsafe(), tag);
        for (const auto& value : values_)
        {
            acc(value.weight, value.value);
        }

        return acc;
    }

    size_t size() const;
    size_t size_unsafe() const;

private:
    void rescale();
    static Weight weight(ElapsedTimeUnit duration);

private:
    mutable Mutex mutex_;
    std::random_device random_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dis_;

    std::vector<Sample> values_;
    Timepoint start_time_;
    Timepoint next_rescale_;
};

template <size_t s, size_t a, typename c, typename m>
const WeightedReservoirTag ExponentiallyDecaying<s, a, c, m>::tag = {};

template <size_t s, size_t a, typename c, typename m>
ExponentiallyDecaying<s, a, c, m>::ExponentiallyDecaying()
: gen_(random_())
, dis_(0, 1)
{
    start_time_ = Clock::now();
    next_rescale_ = start_time_ + RESCALE_INTERVAL;
    values_.reserve(MAX_SIZE);
}

template <size_t s, size_t a, typename c, typename m>
typename ExponentiallyDecaying<s, a, c, m>::Weight
ExponentiallyDecaying<s, a, c, m>::weight(ElapsedTimeUnit duration)
{
    return ::exp(ALPHA * duration.count());
}

template <size_t s, size_t a, typename c, typename m>
size_t ExponentiallyDecaying<s, a, c, m>::size() const
{
    std::lock_guard<m> lock(mutex_);
    return values_.size();
}

template <size_t s, size_t a, typename c, typename m>
size_t ExponentiallyDecaying<s, a, c, m>::size_unsafe() const
{
    return values_.size();
}

template <size_t s, size_t a, typename c, typename m>
void ExponentiallyDecaying<s, a, c, m>::pushValue(double value, Timepoint timestamp)
{
    if(mutex_.try_lock())
    {
        rescale();

        auto elapsed_time =
            std::chrono::duration_cast<ElapsedTimeUnit>(timestamp - start_time_);
        auto exp_weight = weight(elapsed_time);
        auto priority = exp_weight * dis_(gen_);

        if (values_.size() < MAX_SIZE)
        {
            values_.emplace_back(Sample{priority, exp_weight, value});
            std::push_heap(values_.begin(), values_.end());
        }
        else
        {
            if(priority > values_.front().mc_weight)
            {
                std::pop_heap(values_.begin(), values_.end());
                values_.back() = Sample{priority, exp_weight, value};
                std::push_heap(values_.begin(), values_.end());
            }
        }
        mutex_.unlock();
    }
}

template <size_t s, size_t a, typename c, typename m>
void ExponentiallyDecaying<s, a, c, m>::rescale()
{
    Timepoint now = Clock::now();

    if (now < next_rescale_)
    {
        return;
    }

    next_rescale_ = now + RESCALE_INTERVAL;
    auto old_start_time = start_time_;
    start_time_ = now;

    const auto time =
        std::chrono::duration_cast<ElapsedTimeUnit>(start_time_ - old_start_time)
            .count();

    auto scale_factor = ::exp(-ALPHA * time);
    for (auto& v : values_)
    {
        v.mc_weight *= scale_factor;
        v.weight *= scale_factor;
    }
}

} // namespace reservoir
} // namespace metric
} // namespace commonpp
