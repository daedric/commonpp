/*
 * File: src/commonpp/net/http/Request.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/net/http/Request.hpp"

namespace commonpp
{
namespace net
{
namespace http
{

const std::string EOL = "\r\n";

static std::vector<char>& operator<<(std::vector<char>& v, const std::string& str)
{
    v.insert(end(v), begin(str), end(str));
    return v;
}

static std::vector<char>& operator<<(std::vector<char>& v, const char* str)
{
    v.insert(end(v), str, str + ::strlen(str));
    return v;
}

static std::vector<char>& operator<<(std::vector<char>& v,
                                     const std::vector<char>& v2)
{
    v.reserve(v.size() + v2.size());
    v.insert(end(v), begin(v2), end(v2));
    return v;
}

static std::vector<char>& operator<<(std::vector<char>& buff,
                                     const Request::QueryParams& values)
{
    auto it = std::begin(values);
    auto end = std::end(values);

    buff << it->first << "=" << it->second;
    ++it;

    for (; it != end; ++it)
    {
        auto& v = *it;
        buff << "&" << v.first << "=" << v.second;
    }
    return buff;
}
static std::vector<char>& operator<<(std::vector<char>& buff,
                                     const Request::Headers& values)
{
    for (auto& v : values)
    {
        buff << v.first << ": " << v.second << EOL;
    }

    return buff;
}

template <typename T>
static std::vector<char>& operator<<(std::vector<char>& v, T value)
{
    return v << string::stringify(value);
}

std::vector<char> Request::buildRequest() const
{
    std::vector<char> buffer;
    buffer.reserve(BUFSIZ);

    buffer << method_ << " " << path_;

    if (not query_.empty())
    {
        buffer << "?" << query_;
    }

    buffer << " HTTP/" << major_ << "." << minor_ << EOL;
    buffer << headers_;

    if (not body_.empty())
    {
        buffer << "Content-Length: " << string::stringify(body_.size()) << EOL
               << EOL << body_;
    }
    else
    {
        buffer << EOL;
    }

    return buffer;
}

} // namespace http
} // namespace net
} // namespace commonpp
