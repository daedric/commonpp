/*
 * File: include/commonpp/net/http/Request.hpp
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
#include <initializer_list>

#include <boost/algorithm/string.hpp>

#include <commonpp/core/string/stringify.hpp>
#include <commonpp/core/LoggingInterface.hpp>
#include "detail/validators.hpp"

namespace commonpp
{
namespace net
{
namespace http
{

class Request
{
public:
    using VKV = std::vector<std::pair<std::string, std::string>>;
    struct Headers : public VKV
    {
    };
    struct QueryParams : public VKV
    {
    };

    Request(std::string method = "GET")
    : method_(std::move(method))
    {}

    template <typename Validator, typename Serializer>
    struct Wrapper
    {
        template <typename K, typename VT>
        Wrapper& operator+=(std::pair<K, VT> header)
        {
            std::string value = string::stringify(std::move(header.second));

            if (!Validator::validate(header.first, value))
            {
                throw std::invalid_argument("invalid pair");
            }

            vector.emplace_back(Serializer::key(std::move(header.first)),
                                Serializer::value(std::move(value)));
            return *this;
        }

        VKV& vector;
    };

    Request& method(const std::string& method)
    {
        method_ = std::move(method);
        return *this;
    }

    Request& path(std::string path)
    {
        path_ = std::move(path);
        return *this;
    }

    Wrapper<detail::HeaderValidator, detail::NullSerializer> headers()
    {
        return {headers_};
    }

    Wrapper<detail::NullValidator, detail::QuerySerializer> query()
    {
        return {query_};
    }

    template <typename Iterator>
    Request& bodyAppend(Iterator begin, Iterator end)
    {
        auto distance = std::distance(begin, end);
        body_.reserve(body_.size() + distance);
        body_.insert(std::end(body_), begin, end);
        return *this;
    }

    template <typename T>
    Request& bodyAppend(const T& t)
    {
        return bodyAppend(std::begin(t), std::end(t));
    }

    Request& bodyAppend(const char *str)
    {
        auto begin = str;
        auto end = str + ::strlen(str);
        return bodyAppend(begin, end);
    }

    template <typename Iterator>
    Request& body(Iterator begin, Iterator end)
    {
        body_.assign(begin, end);
        return *this;
    }

    template <typename T>
    Request& body(const T& t)
    {
        return body(std::begin(t), std::end(t));
    }

    Request& body(const char* str)
    {
        auto begin = str;
        auto end = str + ::strlen(str);
        return body(begin, end);
    }

    Request& body(std::vector<char> body)
    {
        body_ = std::move(body);
        return *this;
    }

    Request& minor(int m)
    {
        minor_ = m;
        return *this;
    }

    Request& major(int m)
    {
        major_ = m;
        return *this;
    }

    std::vector<char> buildRequest() const;

private:
    int major_ = 1, minor_ = 1;

    std::string method_;
    std::string path_;
    QueryParams query_;
    Headers headers_;
    std::vector<char> body_;
};

} // namespace http
} // namespace net
} // namespace commonpp
