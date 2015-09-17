/*
 * File: include/commonpp/core/traits/conditional.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <type_traits>

namespace commonpp
{
namespace traits
{

template <typename C, typename T, typename F>
struct conditional;

namespace detail
{
template <typename T>
struct get_integral_constant_base
{
    static constexpr const bool has_true_type =
        std::is_base_of<std::true_type, T>::value;

    static constexpr const bool has_false_type =
        std::is_base_of<std::false_type, T>::value;

    static_assert(has_true_type ^ has_false_type,
                  "T needs to inherit from either true_type or false_type");

    using type =
        typename std::conditional<has_true_type, std::true_type, std::false_type>::type;
};

}// namespace detail

template <typename C, typename T, typename F>
struct conditional
    : conditional<typename detail::get_integral_constant_base<C>::type, T, F>
{};

template <typename T, typename F>
struct conditional<std::true_type, T, F> : std::conditional<true, T, F>
{};

template <typename T, typename F>
struct conditional<std::false_type, T, F> : std::conditional<false, T, F>
{};

template <bool C, typename T, typename F>
using conditional_c = std::conditional<C, T, F>;

template <bool C, typename T, typename F>
using conditional_c_t = typename conditional_c<C, T, F>::type;

} // namespace traits
} // namespace commonpp
