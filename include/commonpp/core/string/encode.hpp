/*
 * File: include/commonpp/core/string/encode.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace commonpp
{
namespace string
{
std::string base64_encode(const uint8_t* it, const uint8_t* end);
std::vector<uint8_t> base64_decode(const char* begin, const char* end);

// non-stream based implementation of hex function.
std::string hex_encode(const uint8_t* begin, const uint8_t* end);
std::string hex_encode(const std::vector<uint8_t>& data);

std::vector<uint8_t> hex_decode(const char* begin, const char* end);
std::vector<uint8_t> hex_decode(const std::string& str);

} // namespace string
} // namespace commonpp
