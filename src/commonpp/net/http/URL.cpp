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

#include <cstring>
#include <cstdlib>
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

std::string url_encode(const std::string& url)
{
    std::string result;
    result.reserve(url.size());

    for (uint8_t ch : url)
    {
        if (is_allowed_character(ch))
        {
            result += ch;
        }
        else
        {
            result += "%" + string::hex(&ch, &ch + 1);
        }
    }

    return result;
}

std::string url_decode(const std::string& url)
{
    std::string result;
    result.reserve(url.size());

    auto len = url.size();
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

} // namespace http
} // namespace net
} // namespace commonpp
