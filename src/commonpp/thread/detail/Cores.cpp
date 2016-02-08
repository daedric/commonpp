/*
 * File: src/commonpp/thread/detail/Cores.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
 *
 */

#include "Cores.hpp"

namespace commonpp
{
namespace thread
{
namespace detail
{

Cores::Cores(Options options)
: opt_(options)
{
    hwloc_topology_init(&topology_);
    hwloc_topology_load(topology_);

    get_cores();
}

Cores::~Cores()
{
    if (topology_)
    {
        hwloc_topology_destroy(topology_);
    }
}

Cores::Cores(Cores&& cores)
{
    opt_ = cores.opt_;
    topology_ = cores.topology_;
    cores.topology_ = nullptr;
    nb_cores_ = cores.nb_cores_;
    cores.nb_cores_ = 0;
    get_cores();
}

Cores& Cores::operator=(Cores&& cores)
{
    opt_ = cores.opt_;
    topology_ = cores.topology_;
    cores.topology_ = nullptr;
    nb_cores_ = cores.nb_cores_;
    cores.nb_cores_ = 0;
    get_cores();
    return *this;
}

Cores::Cores(const Cores& cores)
{
    opt_ = cores.opt_;
    hwloc_topology_dup(&topology_, cores.topology_);
    nb_cores_ = cores.nb_cores_;
    get_cores();
}

Cores& Cores:: operator=(const Cores& cores)
{
    opt_ = cores.opt_;
    hwloc_topology_dup(&topology_, cores.topology_);
    nb_cores_ = cores.nb_cores_;
    return *this;
}

void Cores::get_cores()
{
    auto depth = hwloc_get_type_or_below_depth(
        topology_, opt_ == PHYSICAL ? HWLOC_OBJ_CORE : HWLOC_OBJ_PU);
    nb_cores_ = hwloc_get_nbobjs_by_depth(topology_, depth);

    for (size_t i = 0; i < nb_cores_; ++i)
    {
        cores_.emplace_back(topology_,
                            hwloc_get_obj_by_depth(topology_, depth, i));
    }
}

Core& Cores::operator[](int i)
{
    return cores_[i];
}

size_t Cores::cores() const
{
    return nb_cores_;
}

} // namespace detail
} // namespace thread
} // namespace commonpp
