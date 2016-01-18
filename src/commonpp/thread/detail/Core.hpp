/*
 * File: src/commonpp/thread/detail/Core.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
 *
 */

#include <hwloc.h>
#include <thread>

namespace commonpp
{
namespace thread
{
namespace detail
{

class Core
{
public:
    Core(hwloc_topology_t& topology, hwloc_obj_t core);

    hwloc_cpuset_t const& cpuset() const;
    bool bind(std::thread& thread);
    bool bind();

private:
    hwloc_topology_t& topology_;
    hwloc_obj_t core_;
};

} // namespace detail
} // namespace thread
} // namespace commonpp
