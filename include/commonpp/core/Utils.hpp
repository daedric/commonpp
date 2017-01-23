/*
 * File: Utils.hpp
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
#include <functional>
#include <type_traits>
#include <chrono>

namespace commonpp
{

template <typename T>
void fake_delete(T*)
{
}

template <typename T>
void (*get_fake_delete(T*))(T*)
{
    return &fake_delete<T>;
}

template <typename EnumType>
static inline constexpr
    typename std::enable_if<std::is_enum<EnumType>::value,
                            typename std::underlying_type<EnumType>::type>::type
    enum_to_number(EnumType value) noexcept
{
    return static_cast<typename std::underlying_type<EnumType>::type>(value);
}

std::string get_hostname();
std::chrono::seconds get_current_timestamp();

template <typename T, size_t SIZE>
static constexpr size_t get_array_size(T (&)[SIZE]) noexcept
{
    return SIZE;
}

} // namespace commonpp
