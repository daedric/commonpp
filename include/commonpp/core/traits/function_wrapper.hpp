/*
 * File: include/commonpp/core/traits/wrapper.hpp
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

namespace detail
{
template <bool default_return, typename C, typename ReturnType>
struct BoolWrapperCaller
{
    static bool call(C& c)
    {
        return c();
    }
};

template <bool default_return, typename C>
struct BoolWrapperCaller<default_return, C, void>
{
    static bool call(C& c)
    {
        c();
        return default_return;
    }
};
} // namespace detail

template <typename Callable>
bool make_bool_functor(Callable call)
{
    using result = decltype(call());

    if constexpr (std::is_same_v<result, bool>)
    {
        return call();
    }
    else if constexpr (std::is_invocable<Callable>::value)
    {
        call();
        return true;
    }
    else
    {
        static_assert(std::is_invocable<Callable>::value,
                      "Callable is not invocable");
    }
}

} // namespace traits
} // namespace commonpp
