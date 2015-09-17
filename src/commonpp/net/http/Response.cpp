/*
 * File: src/commonpp/net/http/Response.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/net/http/Response.hpp"
#include <cstdlib>
#include <sstream>

namespace commonpp
{
namespace net
{
namespace http
{

Response Response::from(std::vector<char> buffer)
{
    Response result;
    result.buffer_ = std::move(buffer);
    auto& str = boost::get<std::vector<char>>(result.buffer_);
    from(result, str.data(), str.data() + str.size());
    return result;
}

Response Response::from(std::string buffer)
{
    Response result;
    result.buffer_ = std::move(buffer);
    auto& str = boost::get<std::string>(result.buffer_);
    from(result, str.data(), str.data() + str.size());
    return result;
}

static char const MARKER[] = {'\r', '\n', '\r', '\n'};

bool Response::isCompleteHeader(const std::vector<char>& buffer)
{
    if (buffer.size() < 4)
    {
        return false;
    }

    auto buffer_end = buffer.data() + buffer.size();
    return std::search(buffer.data(), buffer_end, std::begin(MARKER),
                       std::end(MARKER)) != buffer_end;
}

bool Response::isCompleteHeader(const std::string& buffer)
{
    if (buffer.size() < 4)
    {
        return false;
    }

    auto buffer_end = buffer.data() + buffer.size();
    return std::search(buffer.data(), buffer_end, std::begin(MARKER),
                       std::end(MARKER)) != buffer_end;
}

static void expect(const char* expect, const char*& it, const char* end)
{
    for (; it != end && *it == *expect; ++it, ++expect)
        ;

    if (*expect == 0)
    {
        return;
    }

    if (it == end)
    {
        throw std::runtime_error("Not enough data to continue");
    }

    std::ostringstream out;
    out << "Invalid character, expected: '" << int(*expect)
        << "' got: " << int(*it);
    throw std::runtime_error(out.str());
}

void Response::parse_status_line(Response& response, const char*& it, const char* end)
{
    if (it == end)
    {
        throw std::runtime_error("Not enough data to parse");
    }

    expect("HTTP/", it, end);
    // pass major
    if (it < end)
    {
        ++it;
    }
    expect(".", it, end);
    // pass minor
    if (it < end)
    {
        ++it;
    }

    char* new_it;
    response.code_ = ::strtol(it, &new_it, 10);

    if (new_it == it)
    {
        throw std::runtime_error("Status code is no parsable as int");
    }

    it = new_it;

    auto msg_end = std::find_if(
        it, end, std::bind(std::equal_to<char>(), '\r', std::placeholders::_1));

    if (msg_end == end)
    {
        throw std::runtime_error("not enough data to read the message");
    }

    while (::isspace(*it) && it < msg_end)
        ++it;

    response.message_ = StringRef(it, std::distance(it, msg_end));
    if (response.message_.empty())
    {
        throw std::runtime_error("Message is required");
    }

    it = msg_end;
    expect("\r\n", it, end);
}

void Response::parse_headers(Response& response, const char*& it, const char* end)
{
    if (it == end)
    {
        return;
    }

    while (true)
    {
        StringRef key;
        StringRef value;

        auto key_end = std::find_if(it, end, [](const char c)
                                    {
                                        return strchr(": \r", c) != nullptr;
                                    });

        key = StringRef(it, std::distance(it, key_end));

        it = key_end;

        if (key.empty())
        {
            break;
        }

        while (::isspace(*it) && it != end)
        {
            ++it;
        }

        if (*it != ':')
        {
            throw std::runtime_error("Invalid header, : expected");
        }

        ++it;

        while (*it == ' ' && it != end)
        {
            ++it;
        }

        auto value_end = std::find(it, end, '\r');
        if (value_end != it)
        {
            value = StringRef(it, std::distance(it, value_end));
            it = value_end;
        }

        response.headers_[key] = std::move(value);

        if (!(*it == '\r' && *(it + 1) == '\n'))
        {
            throw std::runtime_error("Invalid Header, expect \\r\\n at "
                                     "the end of an header");
        }
        ++it;
        ++it;
    }
}

void Response::from(Response& result, const char* begin, const char* end)
{
    parse_status_line(result, begin, end);
    parse_headers(result, begin, end);
    expect("\r\n", begin, end);
}

} // namespace http
} // namespace net
} // namespace commonpp
