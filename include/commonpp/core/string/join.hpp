/*
 * File: include/commonpp/core/string/join.hpp
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
#include <utility>

namespace commonpp
{
namespace string
{

static inline std::string join(char, std::string p)
{
    return p;
}

template <typename... Args>
static inline std::string
join(char j, const std::string& p1, const std::string& p2, Args&&... args)
{
    if ((!p1.empty() && p1[p1.size() - 1] == j) || (!p2.empty() && p2[0] == j))
    {
        return join(j, p1 + p2, std::forward<Args>(args)...);
    }

    return join(j, p1 + j + p2, std::forward<Args>(args)...);
}

} // namespace string
} // namespace commonpp
