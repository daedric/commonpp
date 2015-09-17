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
#include <string>

namespace commonpp
{
namespace string
{
std::string base64_encode(const std::string& str);

// non-stream based implementation of hex function.
std::string hex(const uint8_t* it, const uint8_t* end);
} // namespace string
} // namespace commonpp
