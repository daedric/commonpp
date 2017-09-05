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
#include <commonpp/core/config.hpp>
#include <commonpp/core/Utils.hpp>

#if BOOST_CLANG == 1 && BOOST_MAJOR == 1 && BOOST_MAJOR <= 55
# if defined(BOOST_PP_VARIADICS)
#  if !BOOST_PP_VARIADICS
#   warning BOOST_PP_VARIADICS cannot be defined as 0, redefining it to 1a
#   undef BOOST_PP_VARIADICS
#   define BOOST_PP_VARIADICS 1
#  endif
# else
#  define BOOST_PP_VARIADICS 1
# endif
#endif


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
    BOOST_CXX14_CONSTEXPR Options(E... v) noexcept
    {
        flag_ = set(initial_value..., v...);
    }

    Options& operator&=(Enum f) noexcept
    {
        flag_ |= enum_to_number(f);
        return *this;
    }

    Options& operator+=(Enum f) noexcept
    {
        flag_ |= enum_to_number(f);
        return *this;
    }

    Options& operator-=(Enum f) noexcept
    {
        flag_ = flag_ & ~enum_to_number(f);
        return *this;
    }

    bool operator&(Enum f) const noexcept
    {
        return (flag_ & enum_to_number(f)) != 0;
    }

    bool operator[](Enum f) const noexcept
    {
        return (flag_ & enum_to_number(f)) != 0;
    }

    template <Enum... e>
    bool operator==(const Options<Enum, e...>& rhs) const noexcept
    {
        return flag() == rhs.flag();
    }

    bool empty() const noexcept
    {
        return flag_ == 0;
    }

    Underlying flag() const noexcept
    {
        return flag_;
    }

    void unsafe_set_flag(Underlying u) noexcept
    {
        flag_ = u;
    }

    void reset() noexcept
    {
        flag_ = 0;
    }

private:
    static constexpr Underlying set() noexcept
    {
        return 0;
    }

    static constexpr Underlying set(Enum value) noexcept
    {
        return enum_to_number(value);
    }

    template <typename Head, typename... Tail>
    static constexpr Underlying set(Head value, Tail... tail) noexcept
    {
        return set(value) | set(tail...);
    }

    Underlying flag_ = set(initial_value...);
};

#define COMMONPP_PRINT_OPTION_IF(R, Opts, Elem)                                \
    do                                                                         \
    {                                                                          \
        os << "Flag: " << BOOST_PP_STRINGIZE(Elem) << ": "                     \
           << ((Opts & Elem) ? "Enabled" : "Disabled") << ", ";                \
    } while (0);

#define COMMONPP_DECLARE_OPTIONS_OSTREAM_OP(Options)                           \
    std::ostream& operator<<(std::ostream& os, const Options& opt)

#define COMMONPP_GENERATE_OPTIONS_OSTREAM_OP(Options, ...)                     \
    COMMONPP_DECLARE_OPTIONS_OSTREAM_OP(Options)                               \
    {                                                                          \
        os << BOOST_PP_STRINGIZE(Options) << ": [";                            \
        BOOST_PP_SEQ_FOR_EACH(COMMONPP_PRINT_OPTION_IF, opt,                   \
                              BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))           \
        os << "]";                                                             \
        return os;                                                             \
    }

} // namespace commonpp
