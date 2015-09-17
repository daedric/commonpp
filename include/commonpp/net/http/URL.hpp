/*
 * File: include/commonpp/net/http/URL.hpp
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
namespace net
{
namespace http
{

std::string url_encode(const std::string& url);
std::string url_decode(const std::string& url);

} // namespace http
} // namespace net
} // namespace commonpp
