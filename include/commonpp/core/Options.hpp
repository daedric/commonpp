/*
 * File: include/commonpp/core/Options.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <iosfwd>
#include <type_traits>
#include <commonpp/core/Utils.hpp>

#include <boost/config.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

namespace commonpp
{

template <typename Enum, Enum... initial_value>
struct Options
{
    using Underlying = typename std::underlying_type<Enum>::type;
    static_assert(std::is_unsigned<Underlying>::value, "Enum must be unsigned");

    constexpr Options() = default;

    template <typename... E>
    BOOST_CXX14_CONSTEXPR Options(E... v)
    {
        flag = set(initial_value..., v...);
    }

    Options& operator&=(Enum f)
    {
        flag |= enum_to_number(f);
        return *this;
    }

    Options& operator+=(Enum f)
    {
        flag |= enum_to_number(f);
        return *this;
    }

    Options& operator-=(Enum f)
    {
        flag = flag & ~enum_to_number(f);
        return *this;
    }

    bool operator&(Enum f) const
    {
        return (flag & enum_to_number(f)) != 0;
    }

    bool operator[](Enum f) const
    {
        return (flag & enum_to_number(f)) != 0;
    }

private:
    static constexpr Underlying set()
    {
        return 0;
    }

    static constexpr Underlying set(Enum value)
    {
        return enum_to_number(value);
    }

    template <typename Head, typename... Tail>
    static constexpr Underlying set(Head value, Tail... tail)
    {
        return set(value) | set(tail...);
    }

    Underlying flag = set(initial_value...);
};

#define COMMONPP_PRINT_OPTION_IF(R, Opts, Elem)                                \
    do                                                                         \
    {                                                                          \
        os << "Flag: " << BOOST_PP_STRINGIZE(Elem) << ": "                     \
           << ((Opts & Elem) ? "Enabled" : "Disabled") << ", ";                \
    } while (0);

#define COMMONPP_DECLARE_OPTIONS_OSTREAM_OP(Options)                           \
    std::ostream& operator<<(std::ostream& os, const Options& opt)

#define COMMONPP_GENERATE_OPTIONS_OSTREAM_OP(Options, Enum...)                 \
    COMMONPP_DECLARE_OPTIONS_OSTREAM_OP(Options)                               \
    {                                                                          \
        os << BOOST_PP_STRINGIZE(Options) << ": [";                            \
        BOOST_PP_SEQ_FOR_EACH(COMMONPP_PRINT_OPTION_IF, opt,                   \
                              BOOST_PP_VARIADIC_TO_SEQ(Enum))                  \
        os << "]";                                                             \
        return os;                                                             \
    }

} // namespace commonpp
