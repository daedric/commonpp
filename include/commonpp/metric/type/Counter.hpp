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
#include <type_traits>

#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
# include <tbb/combinable.h>
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
# include <mutex>
# include <commonpp/thread/Spinlock.hpp>
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
# include <atomic>
#endif

#include <commonpp/core/FloatingArithmeticTools.hpp>
#include <commonpp/metric/MetricValue.hpp>
#include <commonpp/metric/detail/types.hpp>

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
    }

    template <typename T,
              typename = typename std::enable_if<std::is_integral<T>::value>::type>
    Counter(T& integer, std::string name = "")
    : value_([&integer] { return integer; })
    , name_(move(name))
    {
    }

    MetricValue getMetrics()
    {
        MetricValue result;
        result.push(value_(), name_);
        return result;
    }

private:
    std::function<Type()> value_;
    std::string name_;
};

class SharedCounter
{
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
    using CounterType = tbb::combinable<uintmax_t>;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    using CounterType = uintmax_t;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
    using CounterType = std::atomic<uintmax_t>;
#endif

public:
    SharedCounter(std::string name = "");

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

    uintmax_t sum() const;
    const std::string& name() const noexcept
    {
        return name_;
    }

private:
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    mutable thread::Spinlock mutex_;
#endif
    mutable CounterType counter_;
    const std::string name_;
};

#undef SHARED_LOCK_USE_TBB
#undef SHARED_LOCK_USE_ATOMIC
#undef SHARED_LOCK_USE_SPINLOCK
#undef SHARED_COUNTER_BACKEND

} // namespace type
} // namespace metric
} // namespace commonpp
