/*
 * File: Counter.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

// Order of efficiency
#define SHARED_LOCK_USE_TBB 1
#define SHARED_LOCK_USE_ATOMIC 2
#define SHARED_LOCK_USE_SPINLOCK 3

#ifndef SHARED_COUNTER_BACKEND
# define SHARED_COUNTER_BACKEND SHARED_LOCK_USE_TBB
#endif

#include <functional>

#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
# include <tbb/combinable.h>
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
# include <mutex>
# include <commonpp/thread/Spinlock.hpp>
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
# include <atomic>
#endif

#include <commonpp/metric/detail/types.hpp>
#include <commonpp/core/FloatingArithmeticTools.hpp>
#include <commonpp/metric/MetricValue.hpp>

namespace commonpp
{
namespace metric
{

class Metrics;

namespace type
{

template <typename Type = uintmax_t>
struct Counter
{
    template <typename Callable>
    Counter(Callable fn, std::string name = "")
    : value_(fn)
    , name_(move(name))
    {
        last_time_point = Clock::now();
    }

    MetricValue getMetrics();

private:
    std::function<Type()> value_;
    std::string name_;
    Type last_value = 0;
    TimePoint last_time_point;
};

class SharedCounter
{
    friend class ::commonpp::metric::Metrics;
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
    using CounterType = tbb::combinable<uintmax_t>;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    using CounterType = uintmax_t;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
    using CounterType = std::atomic<uintmax_t>;
#endif

public:
    SharedCounter();

    void inc(uintmax_t = 1) const;
    void operator++() const
    {
        inc();
    }

    void operator++(int) const
    {
        inc();
    }

    void reset();

private:
    uintmax_t sum() const;

private:
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    mutable thread::Spinlock mutex_;
#endif
    mutable CounterType counter_;
};


// Counter::getMetrics impl
template <typename T>
MetricValue Counter<T>::getMetrics()
{
    MetricValue result;
    auto now  = result.getCapturetime();

    T new_value = value_();

    if (new_value < last_value)
    {
        last_value = new_value;
        last_time_point = now;
        result.push(get_no_metric_marker<T>(), name_);
        return result;
    }

    T delta_value = new_value - last_value;

    double delta_time =
        std::chrono::duration_cast<std::chrono::seconds>(now - last_time_point)
            .count();

    double counter_value;
    if (!(essentially_equal(delta_time, double(0))))
    {
        counter_value = delta_value / delta_time;
    }
    else
    {
        counter_value = get_no_metric_marker<T>();
    }

    last_value = new_value;
    last_time_point = now;
    result.push(counter_value, name_);
    return result;
}

} // namespace type
} // namespace metric
} // namespace commonpp
