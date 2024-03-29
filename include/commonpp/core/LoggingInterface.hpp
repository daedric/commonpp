/*
 * File: include/commonpp/core/LoggingInterface.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#pragma once

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <commonpp/core/config.hpp>

namespace commonpp
{

enum LoggingLevel
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal,
};

std::string_view to_string(LoggingLevel level);
std::ostream& operator<<(std::ostream& strm, LoggingLevel level);

UNUSED_ATTR static LoggingLevel to_severity(int lvl) noexcept
{
    switch (lvl)
    {
    case trace:
        return trace;
    case debug:
        return debug;
    case info:
        return info;
    case warning:
        return warning;
    case error:
        return error;
    case fatal:
        return fatal;
    }

    return info;
}

static inline size_t to_syslog_level(commonpp::LoggingLevel level) noexcept
{
    switch (level)
    {
    case trace:
        return 7;
    case debug:
        return 7;
    case info:
        return 6;
    case warning:
        return 4;
    case error:
        return 3;
    case fatal:
        return 2;
    default:
        return 2;
    }
}

namespace core
{

BOOST_LOG_ATTRIBUTE_KEYWORD(Severity, "Severity", LoggingLevel);
BOOST_LOG_ATTRIBUTE_KEYWORD(Channel, "Channel", std::string);
BOOST_LOG_ATTRIBUTE_KEYWORD(CommonppRecord, "CommonppRecord", bool);

using BasicLogger = boost::log::sources::severity_logger_mt<LoggingLevel>;
using Logger = boost::log::sources::severity_channel_logger_mt<LoggingLevel>;

#define COMPONENT(component_name)                                              \
    (boost::log::keywords::channel = component_name)

#define FWD_DECLARE_LOGGER(l, type)                                            \
    BOOST_LOG_GLOBAL_LOGGER(l##_type, type);                                   \
    extern type l;

#define DECLARE_BASIC_LOGGER(l)                                                \
    BOOST_LOG_GLOBAL_LOGGER_INIT(l##_type, ::commonpp::core::BasicLogger)      \
    {                                                                          \
        ::commonpp::core::BasicLogger logger;                                  \
        logger.add_attribute("CommonppRecord",                                 \
                             boost::log::attributes::constant<bool>(true));    \
        return logger;                                                         \
    }                                                                          \
    ::commonpp::core::BasicLogger l = l##_type::get()

#define DECLARE_LOGGER(l, component_name)                                      \
    BOOST_LOG_GLOBAL_LOGGER_INIT(l##_type, ::commonpp::core::Logger)           \
    {                                                                          \
        ::commonpp::core::Logger logger(COMPONENT(component_name));            \
        logger.add_attribute("CommonppRecord",                                 \
                             boost::log::attributes::constant<bool>(true));    \
        return logger;                                                         \
    }                                                                          \
    ::commonpp::core::Logger l = l##_type::get()

FWD_DECLARE_LOGGER(global_logger, BasicLogger);

#define LOGGER_CTOR_INLINE(component_name)                                     \
    [component_name]                                                           \
    {                                                                          \
        ::commonpp::core::Logger l(COMPONENT(component_name));                 \
        l.add_attribute("CommonppRecord",                                      \
                        boost::log::attributes::constant<bool>(true));         \
        return l;                                                              \
    }()

#define ENABLE_CURRENT_FCT_LOGGING() BOOST_LOG_FUNCTION()

#define CREATE_LOGGER(logger, component_name)                                  \
    BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(                                  \
        logger##_type, ::commonpp::core::Logger, (COMPONENT(component_name))); \
    static auto& logger = []() -> decltype(logger##_type::get())               \
    {                                                                          \
        auto& l = logger##_type::get();                                        \
        l.add_attribute("CommonppRecord",                                      \
                        boost::log::attributes::constant<bool>(true));         \
        return l;                                                              \
    }();

#define CREATE_LOGGER_WITH_INIT(logger, component_name, init_fn_like)          \
    BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(                                  \
        logger##_type, ::commonpp::core::Logger, (COMPONENT(component_name))); \
    static auto& logger = []() -> decltype(logger##_type::get())               \
    {                                                                          \
        auto& l = logger##_type::get();                                        \
        l.add_attribute("CommonppRecord",                                      \
                        boost::log::attributes::constant<bool>(true));         \
        init_fn_like(l);                                                       \
        return l;                                                              \
    }();

#define LOG_SEV(l, s) BOOST_LOG_SEV(l, s)
#define GLOG_SEV(sev) LOG_SEV(::commonpp::core::global_logger, sev)
#define LOG(l, s) BOOST_LOG_SEV(l, ::commonpp::s)
#define GLOG(sev) LOG(::commonpp::core::global_logger, sev)
#define TRACE(sev)                                                             \
    ENABLE_CURRENT_FCT_LOGGING();                                              \
    GLOG(sev)
#define TRACE_LOG(l, sev)                                                      \
    ENABLE_CURRENT_FCT_LOGGING();                                              \
    LOG(l, sev)

// clang-format off
#ifndef NDEBUG
# define DLOG(l, s) BOOST_LOG_SEV(l, commonpp::s)
# define DGLOG(sev) LOG(::commonpp::core::global_logger, sev)
#else
// LOG() and GLOG() are way too complex to be in a ternary expression.
struct NullSink
{
    template <typename T>
    NullSink& operator<<(T&&)
    {
        return *this;
    }
};
// from Google glog
struct Voidify { void operator&(NullSink&){}};
# define DLOG(l, s) true ? (void)0 : commonpp::core::Voidify() & commonpp::core::NullSink()
# define DGLOG(sev) true ? (void)0 : commonpp::core::Voidify() & commonpp::core::NullSink()
#endif
// clang-format on

#define DTRACE(sev)                                                            \
    ENABLE_CURRENT_FCT_LOGGING();                                              \
    DGLOG(sev)
#define DTRACE_LOG(l, sev)                                                     \
    ENABLE_CURRENT_FCT_LOGGING();                                              \
    DLOG(l, sev)

void init_logging();
void enable_console_logging();
void enable_builtin_syslog();
void auto_flush_console(bool b = true);
void set_logging_level(LoggingLevel level);

// default file pattern looks like: file_2008-07-05_13-44-23.1.log
void add_file_sink(const std::string& path = "file_%Y-%m-%d_%H-%M-%S.%N.log ");
void add_file_sink_rotate(
    const std::string& path = "file_%Y-%m-%d_%H-%M-%S.%N.log",
    size_t max_size = 0,
    boost::posix_time::hours period = boost::posix_time::hours(1));

// this should be called before any log happens
void set_logging_level_for_channel(const std::string& channel, LoggingLevel level);

void add_gelf_sink(
    std::string server,
    std::string port,
    std::vector<std::pair<std::string, std::string>> static_fields = {});

} // namespace core
} // namespace commonpp
