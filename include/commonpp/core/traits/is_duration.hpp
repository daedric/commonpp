/*
 * File: include/commonpp/core/traits/is_duration.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#pragma once

#include <chrono>
#include <type_traits>

namespace commonpp
{
namespace traits
{

namespace detail
{
template <typename T>
struct is_duration : std::false_type
{
};

template <typename r, typename p>
struct is_duration<std::chrono::duration<r, p>> : std::true_type
{
};

} // namespace detail

template <typename T>
struct is_duration : detail::is_duration<T>
{
};

} // namespace traits
} // namespace commonpp
