/*
 * File: src/commonpp/core/LoggingInterface.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2018 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/core/LoggingInterface.hpp"
#include "commonpp/core/Utils.hpp"
#include "commonpp/core/config.hpp"
#include "commonpp/core/string/json_escape.hpp"
#include "commonpp/net/Connection.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/core.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/detail/config.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword_fwd.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/formatting_ostream_fwd.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <iostream>

#include <iostream>
#include <random>
#include <stdint.h>
#include <string>

namespace logging = boost::log;
namespace sinks = logging::sinks;
namespace expr = logging::expressions;

namespace commonpp
{
namespace core
{

class GelfFormatter
{
    std::string host_;
    std::vector<std::pair<std::string, std::string>> static_fields_;

public:
    GelfFormatter(std::string host,
                  std::vector<std::pair<std::string, std::string>> static_fields)
    : host_(std::move(host))
    , static_fields_(std::move(static_fields))
    {
        // it might be possible to "pre-render" these to a fragment of json.
        for (auto& s : static_fields_)
        {
            s.first = "_" + commonpp::string::escape_json_string(s.first);
            s.second = commonpp::string::escape_json_string(s.second);
        }
    }

    void operator()(const logging::record_view& rec,
                    logging::formatting_ostream& strm)
    {
        strm << "{";
        strm << "\"version\" : \"1.1\",";
        strm << "\"host\" : \"" << host_ << "\",";
        for (auto s : static_fields_)
        {
            strm << "\"" << s.first << "\" : \"" << s.second << "\",";
        }

        auto severity = rec[Severity];
        if (!severity.empty())
        {
            strm << "\"level\" : " << to_syslog_level(*severity) << ",";
        }
        strm << "\"short_message\" : \""
             << commonpp::string::escape_json_string(*rec[expr::smessage])
             << "\"";
        strm << "}";
    }
};

class GelfUDPBackend
    : public sinks::basic_formatted_sink_backend<char, sinks::synchronized_feeding>
{
    boost::asio::io_service io_service_;
    std::unique_ptr<commonpp::net::UDPConnection> conn_;
    std::string host_;
    std::string port_;
    std::random_device dev_random_;
    std::mt19937_64 rng_;

    // to avoid spamming cerr with logs about a bad send()/connect(), only
    // log every SOCKET_ERROR_LOG_INTERVAL errors
    static constexpr int SOCKET_ERROR_LOG_INTERVAL = 100;
    static constexpr size_t MAX_PACKET_SIZE = 1000;
    static constexpr size_t HEADER_OVERHEAD = 12;

public:
    using base =
        sinks::basic_formatted_sink_backend<char, sinks::synchronized_feeding>;

    GelfUDPBackend(std::string host, std::string port)
    : base()
    , host_(std::move(host))
    , port_(std::move(port))
    {
        rng_.seed(dev_random_());
    }

    void consume(logging::record_view const& rec, string_type const& payload)
    {
        if (payload.size() < MAX_PACKET_SIZE)
        {
            send(payload.data(), payload.size());
        }
        else
        {
            chunk_message(payload);
        }
    }

    void chunk_message(const string_type& payload)
    {
        const auto chunks = 1 + ceil(payload.size() / MAX_PACKET_SIZE);
        if (chunks > 128)
        {
            std::cerr << "Error occured while sending GELF message: Message too big";
            return;
        }
        const auto num_chunks = static_cast<uint8_t>(chunks);

        // setup the header.
        std::array<uint8_t, HEADER_OVERHEAD + MAX_PACKET_SIZE> buffer;
        buffer[0] = 0x1e;
        buffer[1] = 0x0f;
        auto msg_id = get_message_id();
        std::copy_n(reinterpret_cast<const uint8_t*>(&msg_id), 8, &buffer[2]);
        buffer[11] = num_chunks;

        const uint8_t* payload_ptr =
            reinterpret_cast<const uint8_t*>(payload.data());
        size_t remaining = payload.size();

        for (uint8_t chunk_number = 0; chunk_number < num_chunks; chunk_number++)
        {
            buffer[10] = chunk_number;
            const size_t payload_size =
                remaining < MAX_PACKET_SIZE ? remaining : MAX_PACKET_SIZE;
            std::copy_n(payload_ptr, payload_size, &buffer[12]);

            boost::system::error_code ec;
            if (!send(buffer.data(), payload_size + HEADER_OVERHEAD))
            {
                break;
            }

            payload_ptr += payload_size;
            remaining -= payload_size;
        }
    }

    bool send(const void * data, size_t size)
    {	
        if (!conn_)
        {
            try
            { 
                conn_ = commonpp::net::UDPConnection::createConnection(io_service_, host_, port_);
            }
            catch (const std::exception & e)
            {
                std::cerr << "Cannot connect to " << host_ << ":" << port_ << " " << e.what();
                return false;
            }
        }

        boost::system::error_code ec;
        conn_->socket.send(boost::asio::buffer(data, size), 0, ec);
        if (ec)
        {
            std::cerr << "Error occured while sending GELF message: "
                << ec.message() << std::endl;
            conn_.reset();
            return false;
        }
        return true;
    }
    

    uint64_t get_message_id()
    {
        return std::uniform_int_distribution<uint64_t>()(rng_);
    }
};

void add_gelf_sink(std::string server,
                   std::string port,
                   std::vector<std::pair<std::string, std::string>> static_fields)
{
    auto backend = boost::make_shared<GelfUDPBackend>(server, port);
    using sink_t = sinks::synchronous_sink<GelfUDPBackend>;
    boost::shared_ptr<sink_t> sink(new sink_t(backend));

    GelfFormatter fmt(commonpp::get_hostname(), std::move(static_fields));
    sink->set_formatter(fmt);
    sink->set_filter(CommonppRecord.or_default(false) == true);
    logging::core::get()->add_sink(sink);
}

} // namespace core
} // namespace commonpp
