/*
 * File: include/commonpp/net/http/Response.hpp
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
#include <vector>
#include <boost/container/flat_map.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>

namespace commonpp
{
namespace net
{
namespace http
{

class Response
{
public:
    using StringRef = boost::string_ref;
    using Headers = boost::container::flat_map<StringRef, StringRef>;

    int code() const noexcept { return code_; }
    StringRef message() const { return message_; }
    const Headers& headers() const noexcept { return headers_; }

    static bool isCompleteHeader(const std::vector<char>& buffer);
    static bool isCompleteHeader(const std::string& buffer);

    static Response from(std::vector<char> buffer);
    static Response from(std::string buffer);

private:
    static void from(Response&, const char* begin, const char* end);

    static void parse_status_line(Response& , const char*&, const char*);
    static void parse_headers(Response& , const char*&, const char*);

private:
    boost::variant<std::vector<char>, std::string> buffer_;
    int code_ = 200;
    StringRef message_;
    Headers headers_;
};

} // namespace http
} // namespace net
} // namespace commonpp
