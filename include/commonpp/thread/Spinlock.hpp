/*
 * File: Spinlock.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <thread>
#include <iostream>

#define SPINLOCK_USE_ATOMIC 1
#define SPINLOCK_USE_TBB 2

#ifndef SPINLOCK_BACKEND
# define SPINLOCK_BACKEND SPINLOCK_USE_TBB
#endif

#if SPINLOCK_BACKEND == SPINLOCK_USE_ATOMIC
# if defined (__clang__) && (defined (__i386__) || defined (__x86_64__))
#  include <xmmintrin.h>
# endif

# include <atomic>
#elif SPINLOCK_BACKEND == SPINLOCK_USE_TBB
# include <tbb/spin_mutex.h>
#endif

namespace commonpp
{
namespace thread
{

#if SPINLOCK_BACKEND == SPINLOCK_USE_ATOMIC
namespace detail
{
enum LockState
{
    Unlocked = 0,
    Locked = 1,
};

using LockType = std::atomic<LockState>;
struct _spinlock
{
    mutable LockType state;

    bool try_lock() const
    {
        return state.exchange(Locked, std::memory_order_acquire) != Locked;
    }

    void lock() const
    {
        while (!try_lock)
        {
#if defined(__i386__) || defined(__x86_64__)
# ifdef __clang__
            _mm_pause();
# else
            __builtin_ia32_pause();
# endif
#endif
        }
    }

    void unlock() const
    {
        state.store(Unlocked, std::memory_order_release);
    }
};
} // namespace detail
#elif SPINLOCK_BACKEND == SPINLOCK_USE_TBB
namespace detail
{
    using _spinlock = tbb::spin_mutex;
} // namespace detail
#endif

struct Spinlock : private detail::_spinlock
{
    using detail::_spinlock::lock;
    using detail::_spinlock::try_lock;
    using detail::_spinlock::unlock;
};
} // namespace thread
} // namespace commonpp
