/*
 * File: src/commonpp/thread/Thread.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/thread/Thread.hpp"

#include "commonpp/core/config.hpp"

// clang-format off

#if HAVE_HWLOC == 1
# include "detail/Cores.hpp"
#endif

#if HAVE_SYS_PRCTL_H
# include <sys/prctl.h>
# include <cerrno>
# include <cstring>
#endif

#include <sstream>
#include <thread>

#ifndef HAVE_THREAD_LOCAL_SPECIFIER
# include <boost/thread/tss.hpp>
#endif
// clang-format on

#include "commonpp/core/LoggingInterface.hpp"
#include "detail/logger.hpp"

namespace commonpp
{
namespace thread
{

#if HAVE_THREAD_LOCAL_SPECIFIER
static thread_local std::string current_name;
#else
static boost::thread_specific_ptr<std::string> current_name;
#endif

void set_current_thread_name(const std::string& name)
{
#if HAVE_THREAD_LOCAL_SPECIFIER
    current_name = name;
#else
    current_name.reset(new std::string(name));
#endif

#if defined(HAVE_SYS_PRCTL_H)
    auto short_thread_name = name.substr(0, 15);
    if (prctl(PR_SET_NAME, short_thread_name.c_str()))
    {
        LOG(thread_logger, warning)
            << "Cannot set the custom thread name: " << short_thread_name
            << ", errno: " << errno << ", msg: " << strerror(errno);
    }
#endif
}

const std::string& get_current_thread_name()
{
#if HAVE_THREAD_LOCAL_SPECIFIER
    if (!current_name.empty())
    {
        return current_name;
    }
#else
    if (current_name.get())
    {
        return *current_name;
    }
#endif

    std::ostringstream out;
    out << std::this_thread::get_id();
    set_current_thread_name(out.str());

    return get_current_thread_name();
}

#if HAVE_HWLOC == 1
int get_nb_physical_core()
{
    return detail::Cores(detail::Cores::PHYSICAL).cores();
}

int get_nb_logical_core()
{
    return detail::Cores(detail::Cores::ALL).cores();
}

bool set_affinity_to_physical_core(int core)
{
    return detail::Cores(detail::Cores::PHYSICAL)[core].bind();
}

bool set_affinity_to_logical_core(int core)
{
    return detail::Cores(detail::Cores::ALL)[core].bind();
}

bool set_affinity_to_physical_core(int core, std::thread& th)
{
    return detail::Cores(detail::Cores::PHYSICAL)[core].bind(th);
}

bool set_affinity_to_logical_core(int core, std::thread& th)
{
    return detail::Cores(detail::Cores::ALL)[core].bind(th);
}
#else
int get_nb_physical_core()
{
    return std::thread::hardware_concurrency();
}

int get_nb_logical_core()
{
    return std::thread::hardware_concurrency();
}

bool set_affinity_to_physical_core(int)
{
    return false;
}

bool set_affinity_to_logical_core(int)
{
    return false;
}

bool set_affinity_to_physical_core(int, std::thread&)
{
    return false;
}

bool set_affinity_to_logical_core(int, std::thread&)
{
    return false;
}
#endif

} // namespace thread
} // namespace commonpp
