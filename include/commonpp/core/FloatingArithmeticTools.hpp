/*
 * File: Knuth.hpp
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
#include <limits>
#include <cmath>

namespace commonpp
{
// From Knuth
template <typename T, typename U = T>
static inline bool essentially_equal(T a, U b)
{
    static_assert(std::is_floating_point<T>::value,
                  "T must be a floating point number");
    static_assert(std::is_arithmetic<U>::value, "U must be arithmetic");

    using Base = typename std::common_type<T, U>::type;
    static constexpr auto EPSILON = std::numeric_limits<Base>::epsilon();

    return std::fabs(a - b) <=
           ((std::fabs(a) > std::fabs(b) ? std::fabs(b) : std::fabs(a)) * EPSILON);
}

template <typename T, typename U = T>
static inline bool definitly_less_than(T a, U b)
{
    static_assert(std::is_floating_point<T>::value,
                  "T must be a floating point number");
    static_assert(std::is_arithmetic<U>::value, "U must be arithmetic");

    using Base = typename std::common_type<T, U>::type;
    static constexpr auto EPSILON = std::numeric_limits<Base>::epsilon();

    return (b - a) >
           ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * EPSILON);
}
} // namespace commonpp
