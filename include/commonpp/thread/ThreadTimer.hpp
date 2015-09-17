/*
 * File: ThreadTimer.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <commonpp/core/config.hpp>

#if not defined(HAVE_POSIX_CPU_CLOCK)
# error "Current platform cannot use the ThreadTimer"
#endif

#include <ctime>
#include <chrono>
#include <system_error>
#include <thread>
#include <boost/timer/timer.hpp>

namespace commonpp
{
namespace thread
{

class ThreadTimer
{
public:
    ThreadTimer(clockid_t clockId = 0)
    : clock_id_(clockId)
    {
        if (!clock_id_)
        {
            auto self = ::pthread_self();
            if (pthread_getcpuclockid(self, &clock_id_))
            {
                throw std::system_error(errno, std::system_category());
            }
        }

        start();
    }

    void start()
    {
        if (::clock_gettime(clock_id_, &start_time_))
        {
            throw std::system_error(errno, std::system_category());
        }
        cpu_timer_.start();
    }

    std::pair<std::chrono::nanoseconds, std::chrono::nanoseconds> elapsed() const
    {
        ::timespec threadTime;
        if (::clock_gettime(clock_id_, &threadTime))
        {
            throw std::system_error(errno, std::system_category());
        }

        std::chrono::nanoseconds cpu =
            std::chrono::nanoseconds(threadTime.tv_nsec - start_time_.tv_nsec) +
            std::chrono::seconds(threadTime.tv_sec - start_time_.tv_sec);

        auto times = cpu_timer_.elapsed();
        return {cpu, std::chrono::nanoseconds(times.wall)};
    }

private:
    clockid_t clock_id_;
    ::timespec start_time_;
    boost::timer::cpu_timer cpu_timer_; // used for the wall time.
};

} // namespace thread
} // namespace commonpp
