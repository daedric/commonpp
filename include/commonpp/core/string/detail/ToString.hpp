/*
 * File: include/commonpp/core/string/detail/ToString.hpp
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
#include <boost/lexical_cast.hpp>

#include <commonpp/core/traits/conditional.hpp>
#include <commonpp/core/traits/to_string.hpp>

namespace commonpp
{
namespace detail
{

struct StdStringify
{
    template <typename U>
    static auto stringify(U&& std_value)
        -> decltype(std::to_string(std::forward<U>(std_value)))
    {
        return std::to_string(std::forward<U>(std_value));
    }
};

struct NsStringify // Argument-dependent name lookup
{
    template <typename U>
    static auto stringify(U&& value)
        -> decltype(to_string(std::forward<U>(value)))
    {
        return to_string(std::forward<U>(value));
    }
};

struct MemFnStringify
{
    template <typename U>
    static auto stringify(U&& value)
        -> decltype(value.to_string())
    {
        return value.to_string();
    }
};

struct BoostStringify
{
    template <typename U>
    static auto stringify(U&& value)
        -> decltype(boost::lexical_cast<std::string>(std::forward<U>(value)))
    {
        return boost::lexical_cast<std::string>(std::forward<U>(value));
    }
};

template <typename T,
          typename Base = typename traits::conditional<
              traits::has_std_to_string<T>,
              StdStringify,
              typename traits::conditional<
                  traits::has_to_string<T>,
                  NsStringify,
                  typename traits::conditional<traits::has_to_string_memfn<T>,
                                               MemFnStringify,
                                               BoostStringify>::type>::type>::type>
struct ToString : Base
{
};

} // namespace detail
} // namespace commonpp
