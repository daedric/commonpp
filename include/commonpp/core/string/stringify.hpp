/*
 * File: include/commonpp/core/string/stringify.hpp
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

#include "detail/ToString.hpp"

namespace commonpp
{
namespace string
{
template <typename T, typename Type = typename std::decay<T>::type>
static inline auto stringify(T&& t)
    -> decltype(detail::ToString<Type>::stringify(std::forward<T>(t)))
{
    return detail::ToString<Type>::stringify(std::forward<T>(t));
}
} // namespace string
} // namespace commonpp
