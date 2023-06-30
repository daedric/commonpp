/*
 * File: ThreadPool.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <tbb/enumerable_thread_specific.h>

#include <commonpp/core/RandomValuePicker.hpp>
#include <commonpp/core/traits/function_wrapper.hpp>
#include <commonpp/core/traits/is_duration.hpp>

#include "Thread.hpp"

namespace commonpp
{
namespace thread
{

class ThreadPool
{
public:
    using ThreadInit = std::function<void()>;

public:
    using io_context = boost::asio::io_context;
    using executor = io_context::executor_type;
    using deadline_timer = boost::asio::basic_deadline_timer<
        boost::posix_time::ptime,
        boost::asio::time_traits<boost::posix_time::ptime>,
        executor>;
    using TimerPtr = std::shared_ptr<deadline_timer>;

    ThreadPool(size_t nb_thread, std::string name = "", size_t nb_services = 1);
    ThreadPool(size_t nb_thread, io_context& service, std::string name = "");
    ~ThreadPool();

    ThreadPool(ThreadPool&&);
    ThreadPool& operator=(ThreadPool&&) = delete;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    enum
    {
        ROUND_ROBIN = -1,
        RANDOM_SERVICE = -2,
        CURRENT_SERVICE = -3,
    };

    enum class ThreadDispatchPolicy
    {
        Random,
        DispatchToPCore,
        DispatchToAllCore,
    };

    template <typename Callable>
    void post(Callable&& callable, int service_id = ROUND_ROBIN)
    {
        getService(service_id).post(std::forward<Callable>(callable));
    }

    template <typename Callable>
    void dispatch(Callable&& callable, int service_id = ROUND_ROBIN)
    {
        getService(service_id).dispatch(std::forward<Callable>(callable));
    }

    bool runningInPool() const noexcept;
    io_context& getCurrentIOService();

    void start(ThreadInit fct = ThreadInit(),
               ThreadDispatchPolicy policy = ThreadDispatchPolicy::Random);
    void stop();

    boost::asio::io_context& getService(int service_id = ROUND_ROBIN);

    // if callable returns a boolean, if it returns true the timer will be
    // rescheduled automatically
    template <typename Duration, typename Callable>
    TimerPtr schedule(Duration delay,
                      Callable&& callable,
                      int service_id = ROUND_ROBIN);

    size_t threads() const noexcept;

    template <typename Callable>
    void postAll(Callable callable);

    template <typename Callable>
    void dispatchAll(Callable callable);

    // called by each thread before exiting
    void set_cleanup_fn(std::function<void()> cleanup_fn);

private:
    template <typename Duration, typename Callable>
    void schedule_timer(TimerPtr& timer, Duration, Callable&& callable);

    void run(io_context&, ThreadInit fct);

private:
    bool running_ = false;
    const size_t nb_thread_;
    const size_t nb_services_;
    std::string name_;

    std::atomic_uint current_service_{0};
    std::atomic_uint running_threads_{0};

    std::vector<std::thread> threads_;
    std::vector<std::shared_ptr<io_context>> services_;
    std::vector<boost::asio::executor_work_guard<executor>> works_;

    tbb::enumerable_thread_specific<decltype(createPicker(services_))> picker_;

    std::function<void()> on_exit_thread_fn;
};

template <typename Duration, typename Callable>
void ThreadPool::schedule_timer(TimerPtr& timer, Duration delay, Callable&& callable)
{
    timer->expires_from_now(boost::posix_time::milliseconds(
        std::chrono::duration_cast<std::chrono::milliseconds>(delay).count()));
    timer->async_wait(
        [this, delay, timer, callable](const boost::system::error_code& error) mutable
        {
            if (error)
            {
                if (error != boost::asio::error::operation_aborted)
                {
                    throw std::runtime_error(error.message());
                }

                return;
            }

            if (traits::make_bool_functor(callable))
            {
                schedule_timer(timer, delay, callable);
            }
        });
}

template <typename Duration, typename Callable>
ThreadPool::TimerPtr ThreadPool::schedule(Duration delay,
                                          Callable&& callable,
                                          int service_id)
{
    static_assert(traits::is_duration<Duration>::value,
                  "A std::chrono::duration is expected here");
    TimerPtr timer = std::make_shared<deadline_timer>(getService(service_id));
    schedule_timer(timer, delay, std::forward<Callable>(callable));
    return timer;
}

template <typename Callable>
void ThreadPool::postAll(Callable callable)
{
    for (size_t i = 0; i < nb_thread_; ++i)
    {
        post(callable);
    }
}

template <typename Callable>
void ThreadPool::dispatchAll(Callable callable)
{
    for (size_t i = 0; i < nb_thread_; ++i)
    {
        post(callable);
    }
}

} // namespace thread
} // namespace commonpp
