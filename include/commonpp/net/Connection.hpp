/*
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2018 Thomas Sanchez.  All rights reserved.
 *
 */

#include <memory>
#include <boost/asio.hpp>


namespace commonpp
{
namespace net
{

template <typename Protocol>
struct Connection
{
    Connection(boost::asio::io_service& service)
    : socket(service)
    {
    }

    ~Connection()
    {
        boost::system::error_code code;
        socket.shutdown(socket.shutdown_both, code);
    }

    static std::unique_ptr<Connection<Protocol>>
    createConnection(boost::asio::io_service& service,
                     const std::string& host,
                     const std::string& port)
    {
        typename Protocol::resolver resolver(service);
        typename Protocol::resolver::query query(Protocol::v4(), host, port);
        typename Protocol::resolver::iterator iterator = resolver.resolve(query);
        std::unique_ptr<Connection<Protocol>> conn(new Connection<Protocol>(service));
        boost::asio::connect(conn->socket, iterator);
        return conn;
    } 

    typename Protocol::socket socket;
};

using UDPConnection = Connection<boost::asio::ip::udp>;
using TCPConnection = Connection<boost::asio::ip::tcp>;

} // net
} // commonpp
