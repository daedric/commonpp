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

// make any callable a function that returns bool
template <bool default_return,
          typename Callable,
          typename ResultType = typename std::result_of<Callable()>::type>
bool make_bool_functor(Callable&& call)
{
    return detail::BoolWrapperCaller<default_return, Callable, ResultType>::call(
        call);
}

template <typename Callable,
          typename ResultType = typename std::result_of<Callable()>::type>
bool make_bool_functor(Callable&& call)
{
    return detail::BoolWrapperCaller<true, Callable, ResultType>::call(call);
}

} // namespace traits
} // namespace commonpp
