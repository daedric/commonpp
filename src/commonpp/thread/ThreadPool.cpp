/*
 * File: /home/thomas/perso/commonpp/src/commonpp/thread/ThreadPool.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/thread/ThreadPool.hpp"

#include <atomic>
#include <functional>

#include "commonpp/core/LoggingInterface.hpp"
#include "commonpp/core/Utils.hpp"

namespace commonpp
{
namespace thread
{

CREATE_LOGGER(thread_logger, "commonpp::thread");

ThreadPool::ThreadPool(size_t nb_thread, std::string name, size_t nb_services)
: nb_thread_(nb_thread)
, nb_services_(nb_services)
, name_(std::move(name))
, picker_([this]
          {
              return createPicker(services_);
          })
{
    if (nb_services < 1)
    {
        throw std::invalid_argument("nb_services must be > 0");
    }

    if (nb_services > nb_thread_)
    {
        throw std::invalid_argument("not enough thread configured, at least 1 "
                                    "thread by service is required");
    }

    if (nb_thread_ % nb_services != 0)
    {
        throw std::invalid_argument(
            "All the services should have the same number of thread");
    }

    services_.reserve(nb_services);
    works_.reserve(nb_services);
    const std::size_t concurrency_hint = nb_thread / nb_services;
    std::generate_n(std::back_inserter(services_), nb_services, [concurrency_hint]
                    {
                        return std::make_shared<boost::asio::io_service>(concurrency_hint);
                    });
   }

   ThreadPool::ThreadPool(size_t nb_thread,
                          boost::asio::io_service& service,
                          std::string name)
   : nb_thread_(nb_thread)
   , nb_services_(1)
   , name_(std::move(name))
   , services_{
         {
             std::addressof(service), get_fake_delete(std::addressof(service)),
         },
     }
{
}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

ThreadPool::ThreadPool(ThreadPool&& pool)
: running_(pool.running_)
, nb_thread_(pool.nb_thread_)
, nb_services_(pool.nb_services_)
, name_(move(pool.name_))
, threads_(move(pool.threads_))
, services_(move(pool.services_))
, works_(move(pool.works_))
, picker_([this] { return createPicker(services_); })
{
    running_threads_.store(pool.running_threads_.load());
    pool.running_threads_ = 0;
    pool.picker_.clear();
    pool.running_ = false;
}

void ThreadPool::start(ThreadInit fct)
{
    if (running_)
    {
        return;
    }

    int i = 0;
    std::generate_n(std::back_inserter(works_), nb_services_, [this, &i]
                    {
                        return std::unique_ptr<boost::asio::io_service::work>(
                            new boost::asio::io_service::work(*services_[i++]));
                    });

    threads_.reserve(nb_thread_);
    for (size_t i = 0; i < nb_thread_; ++i)
    {
        threads_.emplace_back(
            &ThreadPool::run, this, std::ref(getService(i % nb_services_)),
            [this, fct, i]
            {
                auto suffix = "#" + std::to_string(i) + "|S#" +
                              std::to_string(i % nb_services_);
                if (name_.empty())
                {
                    std::string default_name = "PTH" + suffix;
                    set_current_thread_name(default_name);
                }
                else
                {
                    set_current_thread_name(name_ + suffix);
                }

                if (fct)
                {
                    fct();
                }
            });
    }

    while (running_threads_ != nb_thread_)
    {
        std::this_thread::yield();
    }

    running_ = true;
}

void ThreadPool::run(boost::asio::io_service& service, ThreadInit fct)
{
    if (fct)
    {
        fct();
    }

    ++running_threads_;

    LOG(thread_logger, debug) << "Start thread";

    service.reset();
    service.run();
    --running_threads_;

    LOG(thread_logger, debug) << "Thread stopped";
}

void ThreadPool::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;
    works_.clear();
    picker_.clear();

    for (auto& service : services_)
    {
        service->stop();
    }

    for (auto& th : threads_)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    threads_.clear();
}

boost::asio::io_service& ThreadPool::getService(int service_id)
{
    if (nb_services_ == 1)
    {
        return *services_.front();
    }

    switch (service_id)
    {
    case ROUND_ROBIN:
        return *services_[current_service_++ % nb_services_];
    case RANDOM_SERVICE:
        return *(picker_.local()());
    case CURRENT_SERVICE:
        return getCurrentIOService();
    default:
        return *services_[service_id];
    }
}

boost::asio::io_service& ThreadPool::getCurrentIOService()
{
    int i = 0;
    auto current_id = std::this_thread::get_id();
    for (const auto& thread : threads_)
    {
        if (thread.get_id() == current_id)
        {
            return *services_[i % nb_services_];
        }
        ++i;
    }

    throw std::runtime_error(
        "There is no io service associated with the current thread");
}

bool ThreadPool::runningInPool() const noexcept
{
    auto current_id = std::this_thread::get_id();
    for (const auto& thread : threads_)
    {
        if (thread.get_id() == current_id)
        {
            return true;
        }
    }

    return false;
}

} // namespace thread
} // namespace commonpp
