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
#include <thread>

namespace commonpp
{
namespace thread
{
void set_current_thread_name(const std::string& name);
const std::string& get_current_thread_name();

// The following depends on the hwloc lib being present
// returns -1 if we can't calculate it.
int get_nb_physical_core();
int get_nb_logical_core();

bool set_affinity_to_physical_core(int core);
bool set_affinity_to_logical_core(int core);

bool set_affinity_to_physical_core(int core, std::thread&);
bool set_affinity_to_logical_core(int core, std::thread&);

} // namespace thread
} // namespace commonpp
