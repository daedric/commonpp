/*
 * File: Graphite.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/metric/sink/Graphite.hpp"

#include <algorithm>
#include <functional>
#include <thread>

#include <boost/asio.hpp>

#include "commonpp/core/LoggingInterface.hpp"

namespace commonpp
{
namespace metric
{
namespace sink
{

CREATE_LOGGER(graphite_logger, "commonpp::metric::sink::graphite");

struct Connection
{
    Connection(boost::asio::io_service& service)
    : socket(service)
    {
    }

    ~Connection()
    {
        boost::system::error_code code;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, code);
    }

    static std::unique_ptr<Connection>
    createConnection(boost::asio::io_service& service,
                     const std::string& host,
                     const std::string& port)
    {
        std::unique_ptr<Connection> conn(new Connection(service));

        try
        {
            using boost::asio::ip::tcp;
            tcp::resolver resolver(service);
            tcp::resolver::query query(tcp::v4(), host, port);
            tcp::resolver::iterator iterator = resolver.resolve(query);
            boost::asio::connect(conn->socket, iterator);
            return conn;
        }
        catch (const std::exception& exc)
        {
            LOG(graphite_logger, error)
                << "Cannot establish a connection to graphite: " << exc.what();
            return {};
        }
    }

    boost::asio::ip::tcp::socket socket;
};

Graphite::Graphite(boost::asio::io_service& service,
                   MetricTag prefix,
                   std::string host,
                   std::string port)
: io_service_(service)
, prefix_(std::move(prefix))
, host_(std::move(host))
, port_(std::move(port))
, connection_(Connection::createConnection(service, host_, port_))
{
}

Graphite::~Graphite()
{
}

void Graphite::operator()(const Metrics::MetricVector& values)
{
    if (!connection_)
    {
        connection_ = Connection::createConnection(io_service_, host_, port_);
    }

    if (!connection_)
    {
        return;
    }

    std::string data;

    for (const auto& m : values)
    {
        const auto& tag = std::get<0>(m).get();
        auto& value = std::get<1>(m);

        data += value.toGraphiteFormat(tag, prefix_);
    }

    try
    {
        boost::asio::write(connection_->socket, boost::asio::buffer(data));
    }
    catch (const boost::system::system_error& ex)
    {
        LOG(graphite_logger, error)
            << "An error happened while sending data to graphite: " << ex.what();
        connection_.reset();
    }
}

} // namespace sink
} // namespace metric
} // namespace commonpp
