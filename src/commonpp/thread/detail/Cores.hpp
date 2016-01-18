/*
 * File: src/commonpp/thread/detail/Cores.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
 *
 */

#include <hwloc.h>
#include <vector>

#include "Core.hpp"

namespace commonpp
{
namespace thread
{
namespace detail
{

class Cores
{
public:
    enum Options
    {
        ALL,
        PHYSICAL,
    };

    Cores(Options options = PHYSICAL);
    ~Cores();

    Cores(const Cores&);
    Cores& operator=(const Cores&);

    Cores(Cores&&) ;
    Cores& operator=(Cores&&) ;

    Core& operator[](int i);
    size_t cores() const;

private:
    hwloc_topology_t topology_;
    size_t nb_cores_;

    std::vector<Core> cores_;
};

} // namespace detail
} // namespace thread
} // namespace commonpp
