/*
 * File: Thread.hpp
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
namespace thread
{
void set_current_thread_name(const std::string& name);
const std::string& get_current_thread_name();
} // namespace thread
} // namespace commonpp
