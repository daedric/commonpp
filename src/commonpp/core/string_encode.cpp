/*
 * File: string_encode.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include "commonpp/core/string/encode.hpp"

#include <cstdlib>
#include <stdexcept>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace commonpp
{
namespace string
{

static const char* HEXTABLE[256] = {
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b",
    "0c", "0d", "0e", "0f", "10", "11", "12", "13", "14", "15", "16", "17",
    "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "20", "21", "22", "23",
    "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b",
    "3c", "3d", "3e", "3f", "40", "41", "42", "43", "44", "45", "46", "47",
    "48", "49", "4a", "4b", "4c", "4d", "4e", "4f", "50", "51", "52", "53",
    "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b",
    "6c", "6d", "6e", "6f", "70", "71", "72", "73", "74", "75", "76", "77",
    "78", "79", "7a", "7b", "7c", "7d", "7e", "7f", "80", "81", "82", "83",
    "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b",
    "9c", "9d", "9e", "9f", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af", "b0", "b1", "b2", "b3",
    "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
    "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb",
    "cc", "cd", "ce", "cf", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
    "d8", "d9", "da", "db", "dc", "dd", "de", "df", "e0", "e1", "e2", "e3",
    "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb",
    "fc", "fd", "fe", "ff",
};

std::string hex_encode(const std::vector<uint8_t>& v)
{
    return hex_encode(v.data(), v.data() + v.size());
}

std::string hex_encode(const uint8_t* it, const uint8_t* end)
{
    std::string str;
    str.reserve((end - it) * 2);
    for (; it != end; ++it)
    {
        str += HEXTABLE[*it];
    }

    return str;
}

std::vector<uint8_t> hex_decode(const std::string& str)
{
    return hex_decode(str.data(), str.data() + str.size());
}

std::vector<uint8_t> hex_decode(const char* it, const char* end)
{
    std::vector<uint8_t> result;
    auto size = std::distance(it, end);

    if (size % 2 != 0)
    {
        throw std::invalid_argument(
            "the distance between the two iterators must be even");
    }

    result.reserve(size / 2);
    char tmp[3] = {0, 0, 0};
    for (; it != end;)
    {
        tmp[0] = *it++;
        tmp[1] = *it++;
        auto val = std::strtoul(tmp, nullptr, 16);
        uint8_t* data = reinterpret_cast<uint8_t*>(&val);
        result.emplace_back(data[0]);
    }

    return result;
}

namespace bai = boost::archive::iterators;

// inspiration from here: http://stackoverflow.com/a/16775827
std::string base64_encode(const uint8_t* it, const uint8_t* end)
{
    static const char* padding[] = {"", "==", "="};

    using encoder =
        bai::base64_from_binary<bai::transform_width<const char*, 6, 8>>;

    std::string result;
    std::copy(encoder(it), encoder(end), std::back_inserter(result));

    result += padding[std::distance(it, end) % 3];

    return result;
}

std::vector<uint8_t> base64_decode(const char* begin, const char* end)
{
    std::vector<uint8_t> result;

    using decoder =
        bai::transform_width<bai::binary_from_base64<const char*>, 8, 6>;

    auto size = std::distance(begin, end);

    if (!size)
    {
        return result;
    }

    if (begin[size - 1] == '=')
    {
        --size;
        if (size && begin[size - 1] == '=')
        {
            --size;
        }
    }

    std::copy(decoder(begin), decoder(begin + size), std::back_inserter(result));

    return result;
}

} // namespace string
} // namespace commonpp
