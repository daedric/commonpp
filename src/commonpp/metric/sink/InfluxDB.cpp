/*
 * File: src/commonpp/metric/sink/InfluxDB.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/metric/sink/InfluxDB.hpp"

#include <algorithm>
#include <functional>
#include <thread>

#include <boost/asio.hpp>

#include "commonpp/core/LoggingInterface.hpp"
#include "commonpp/net/http/Response.hpp"

namespace commonpp
{
namespace metric
{
namespace sink
{

CREATE_LOGGER(influxdb_logger, "commonpp::metric::sink::influxdb");

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
            LOG(influxdb_logger, error)
                << "Cannot establish a connection to influxdb: " << exc.what();
            return {};
        }
    }

    boost::asio::ip::tcp::socket socket;
};

InfluxDB::InfluxDB(boost::asio::io_service& service,
                   MetricTag prefix,
                   std::string db,
                   std::string host,
                   std::string port)
: io_service_(service)
, prefix_(std::move(prefix))
, host_(std::move(host))
, port_(std::move(port))
, db_(std::move(db))
, request_("POST")
, connection_(Connection::createConnection(service, host_, port_))
{
    request_.path("/write");
    request_.query() += std::make_pair("db", db_);
    request_.headers() += std::make_pair("Host", host_ + ":" + port_);
}

InfluxDB::~InfluxDB()
{
}

void InfluxDB::operator()(const Metrics::MetricVector& values)
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

        data += value.toInfluxFormat(tag, prefix_);
    }

    request_.body(data);

    try
    {
        auto data = request_.buildRequest();
        boost::asio::write(connection_->socket, boost::asio::buffer(data));

        size_t current_offset = 0;
        data.clear();
        do
        {
            data.resize(data.size() + BUFSIZ);
            if (data.size() > 10 * 1024 * 1024)
            {
                throw std::runtime_error(
                    "Response from influx db is too big > 10M");
            }

            size_t read_data = connection_->socket.read_some(
                boost::asio::buffer(data.data() + current_offset, BUFSIZ));
            current_offset += read_data;
            data.resize(current_offset);
        } while (not net::http::Response::isCompleteHeader(data));

        auto response = net::http::Response::from(std::move(data));
        if (response.code() != 204)
        {
            throw std::runtime_error("Got an invalid response from influxdb: " +
                                     std::to_string(response.code()) + " " +
                                     response.message().to_string());
        }
    }
    catch (const std::exception& ex)
    {
        LOG(influxdb_logger, error)
            << "An error happened while sending data to influxdb: " << ex.what();
        connection_.reset();
    }
}

} // namespace sink
} // namespace metric
} // namespace commonpp
