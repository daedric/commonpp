/*
 * File: Counter.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/metric/type/Counter.hpp"

namespace commonpp
{
namespace metric
{
namespace type
{

SharedCounter::SharedCounter(std::string name)
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
: counter_([] { return uintmax_t(0); })
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
: counter_(0)
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
: counter_(0)
#endif
, name_(std::move(name))
{
}

void SharedCounter::reset()
{
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
    counter_.clear();
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    std::lock_guard<thread::Spinlock> lock(mutex_);
    counter_ = 0;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
    counter_ = 0;
#endif
}

void SharedCounter::inc(uintmax_t inc) const
{
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
    counter_.local() += inc;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    std::lock_guard<thread::Spinlock> lock(mutex_);
    counter_ += inc;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
    counter_ += inc;
#endif
}

uintmax_t SharedCounter::sum() const
{
#if SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_TBB
    return counter_.combine(
        [](uintmax_t lhs, uintmax_t rhs) { return lhs + rhs; });
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_SPINLOCK
    std::lock_guard<thread::Spinlock> lock(mutex_);
    return counter_;
#elif SHARED_COUNTER_BACKEND == SHARED_LOCK_USE_ATOMIC
    return counter_.load();
#endif
}

} // namespace type
} // namespace metric
} // namespace commonpp
