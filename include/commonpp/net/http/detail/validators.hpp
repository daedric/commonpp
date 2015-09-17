/*
 * File: include/commonpp/net/http/detail/validators.hpp
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


#include <boost/algorithm/string.hpp>

#include <commonpp/core/string/stringify.hpp>
#include <commonpp/core/LoggingInterface.hpp>
#include <commonpp/net/http/URL.hpp>

#include "logger.hpp"

namespace commonpp
{
namespace net
{
namespace http
{
namespace detail
{

struct NullValidator
{
    template <typename K, typename V>
    static bool validate(const K&, const V&)
    {
        return true;
    }
};

struct NullSerializer
{
    static std::string key(std::string key)
    {
        return key;
    }

    static std::string value(std::string value)
    {
        return value;
    }
};

struct HeaderValidator
{
    static bool validate(const std::string& key, const std::string&)
    {
        if (boost::iequals(key, "Content-Length"))
        {
            LOG(http_net_logger, warning)
                << "Content-Length is handled by the library";
            return false;
        }
        return true;
    }
};

struct QuerySerializer
{
    static std::string key(const std::string& key)
    {
        return url_encode(key);
    }

    static std::string value(const std::string& value)
    {
        return url_encode(value);
    }
};

} // namespace detail
} // namespace http
} // namespace net
} // namespace commonpp
