/*
 * File: Graphite.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>

#include <boost/asio/ip/tcp.hpp>
#include <commonpp/metric/detail/types.hpp>
#include <commonpp/metric/Metrics.hpp>

namespace commonpp
{
namespace metric
{
namespace sink
{
struct Connection;

class Graphite
{
public:
    Graphite(boost::asio::io_service& service,
             MetricTag prefix,
             std::string host,
             std::string port);

    ~Graphite();

    void operator()(const Metrics::MetricVector& values);

private:
    boost::asio::io_service& io_service_;
    const MetricTag prefix_;
    std::string host_;
    std::string port_;

    std::unique_ptr<Connection> connection_;
};

} // namespace sink
} // namespace metric
} // namespace commonpp
