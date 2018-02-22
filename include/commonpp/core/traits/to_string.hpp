/*
 * File: include/commonpp/core/traits/to_string.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#pragma once

#include "conditional.hpp"
#include <string>
#include <type_traits>

namespace commonpp
{
namespace traits
{

namespace detail
{

template <typename Type>
static constexpr auto has_std_to_string(int)
    -> decltype(std::to_string(std::declval<Type>()), true)
{
    return true;
}

template <typename>
static constexpr bool has_std_to_string(long)
{
    return false;
}

template <typename Type>
static constexpr auto has_to_string(int)
    -> decltype(to_string(std::declval<Type>()), true)
{
    return true;
}

template <typename Type>
static constexpr bool has_to_string(long)
{
    return false;
}

template <typename Type>
static constexpr auto has_to_string_memfn(int)
    -> decltype(std::declval<Type>().to_string(), true)
{
    return true;
}

template <typename Type>
static constexpr bool has_to_string_memfn(long)
{
    return false;
}

} // namespace detail

template <typename T>
struct has_std_to_string
: conditional_c_t<detail::has_std_to_string<T>(0), std::true_type, std::false_type>
{
};

template <typename T>
struct has_to_string
: conditional_c_t<detail::has_to_string<T>(0), std::true_type, std::false_type>
{
};

template <typename T>
struct has_to_string_memfn
: conditional_c_t<detail::has_to_string_memfn<T>(0), std::true_type, std::false_type>
{
};

} // namespace traits
} // namespace commonpp
