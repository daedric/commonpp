/*
 * File: src/commonpp/thread/detail/Core.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
 *
 */

#include "logger.hpp"
#include "Core.hpp"

namespace commonpp
{
namespace thread
{
namespace detail
{

Core::Core(hwloc_topology_t& topology, hwloc_obj_t core)
: topology_(topology)
, core_(core)
{}

hwloc_cpuset_t const& Core::cpuset() const
{
    return core_->cpuset;
}

bool Core::bind(std::thread& thread)
{
    auto cpuset = hwloc_bitmap_dup(core_->cpuset);
    hwloc_bitmap_singlify(cpuset);

    if (hwloc_set_thread_cpubind(topology_, thread.native_handle(), cpuset, 0))
    {
        auto error = errno;
        LOG(thread_logger, warning) << "Error setting thread affinity: "
                                    << strerror(error);
        hwloc_bitmap_free(cpuset);
        return false;
    }

    hwloc_bitmap_free(cpuset);
    return true;
}

bool Core::bind()
{
    auto cpuset = hwloc_bitmap_dup(core_->cpuset);
    hwloc_bitmap_singlify(cpuset);

    if (hwloc_set_cpubind(topology_, cpuset, 0))
    {
        auto error = errno;
        LOG(thread_logger, warning) << "Error setting thread affinity: "
                                    << strerror(error);
        hwloc_bitmap_free(cpuset);
        return false;
    }

    hwloc_bitmap_free(cpuset);
    return true;
}

} // namespace detail
} // namespace thread
} // namespace commonpp
