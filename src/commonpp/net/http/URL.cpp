/*
 * File: src/commonpp/net/http/URL.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/net/http/URL.hpp"

#include <cstdlib>
#include <cstring>

#include "commonpp/core/string/encode.hpp"

namespace commonpp
{
namespace net
{
namespace http
{

static inline bool is_allowed_character(uint8_t c)
{
    static const char* EXTRA = "-_.!~*\\()";
    return ::isalnum(c) || strchr(EXTRA, c) != nullptr;
}

std::string url_encode(const char* begin, const char* end)
{
    std::string result;
    result.reserve(std::distance(begin, end));

    for (auto it = begin; it != end; ++it)
    {
        uint8_t ch = *it;

        if (is_allowed_character(ch))
        {
            result += ch;
        }
        else
        {
            result += "%" + string::hex_encode(&ch, &ch + 1);
        }
    }

    return result;
}

std::string url_decode(const char* begin, const char* end)
{
    std::string result;

    size_t len = std::distance(begin, end);
    result.reserve(len);

    auto url = begin;
    for (size_t i = 0; i < len; ++i)
    {
        auto current_char = url[i];

        if (current_char == '%')
        {
            char n[3];
            n[0] = url[i + 1];
            n[1] = url[i + 2];
            n[2] = 0;

            uint8_t decoded_c = strtol(n, nullptr, 16);
            result += decoded_c;

            i += 2;
        }
        else if (current_char == '+')
        {
            result += ' ';
        }
        else
        {
            result += current_char;
        }
    }

    return result;
}

std::string url_encode(const std::string& url)
{
    return url_encode(url.data(), url.data() + url.size());
}

std::string url_decode(const std::string& url)
{
    return url_decode(url.data(), url.data() + url.size());
}

} // namespace http
} // namespace net
} // namespace commonpp
