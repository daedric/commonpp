/*
 * File: src/commonpp/core/LoggingInterface.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#include "commonpp/core/LoggingInterface.hpp"

#include <cstdlib>
#include <iostream>
#include <thread>

#include <boost/container/flat_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/attributes/function.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions/formatters/named_scope.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/value_ref.hpp>

#include <boost/phoenix/bind.hpp>

#include "commonpp/core/Utils.hpp"
#include "commonpp/thread/Thread.hpp"

namespace logging = boost::log;
namespace sinks = logging::sinks;
namespace keywords = logging::keywords;
namespace expr = logging::expressions;
namespace attrs = logging::attributes;
namespace src = logging::sources;

namespace commonpp
{

std::string_view to_string(LoggingLevel level)
{

    static const char* LEVELS[] = {"trace",   "debug", "info",
                                   "warning", "error", "fatal"};

    if (BOOST_LIKELY(static_cast<std::size_t>(level) < get_array_size(LEVELS)))
    {
        return LEVELS[level];
    }
    return "UNKNOWN LEVEL";
}

std::ostream& operator<<(std::ostream& os, LoggingLevel level)
{
    return os << to_string(level);
}
namespace core
{

DECLARE_BASIC_LOGGER(global_logger);

using FileSink = sinks::synchronous_sink<sinks::text_file_backend>;

using SeverityByChannel =
    expr::channel_severity_filter_actor<std::string, LoggingLevel>;
static SeverityByChannel severity_by_channel =
    expr::channel_severity_filter(Channel, Severity);

static void test_boost_filesystem_locale()
{
    try
    {
        boost::filesystem::path p("/tmp");
        auto relative_path = p.relative_path();
    }
    catch (const std::runtime_error&)
    {
        BOOST_LOG_TRIVIAL(fatal) << "Locale improperly configured";
        abort();
    }
}

static bool filter(logging::attribute_value_set const& values,
                   LoggingLevel global_level)
{
    auto severity = values[Severity];
    if (!severity.empty())
    {
        if (severity < global_level)
        {
            return false;
        }

        if (!severity_by_channel(values))
        {
            return false;
        }
    }

    return true; // by default, let's not loose any logs
}

// clang-format off
static const auto formatter = expr::stream
                 << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                 << " [" << Severity << "]["
                 << expr::if_(expr::has_attr<std::string>("Channel"))
                            [
                                expr::stream << Channel
                            ]
                        .else_
                            [
                                expr::stream << "N/A"
                            ]
                << "][" << expr::attr<std::string>("ThreadName") << "]"
                << expr::if_(expr::has_attr<attrs::named_scope::value_type>("Scope"))
                            [
                                expr::stream
                                    << expr::format_named_scope("Scope",
                                                                keywords::format = "[%n @ %f:%l]")
                            ]
                << expr::if_(expr::has_attr<boost::posix_time::time_duration>("Duration"))
                            [
                                expr::stream << "[ExecTime: " << expr::attr<boost::posix_time::time_duration>("Duration") << "]"
                            ]
                << ": " << expr::message;
// clang-format on

static decltype(boost::log::add_console_log()) console_logger = nullptr;

#if HAVE_SYSLOG
using SyslogSink =
    boost::shared_ptr<sinks::synchronous_sink<sinks::syslog_backend>>;
static SyslogSink syslog_logger = nullptr;
#endif

static void flush_logs()
{
    auto core = logging::core::get();
    core->flush();
    core->remove_all_sinks();
}

void init_logging()
{
    test_boost_filesystem_locale();

    boost::log::add_common_attributes();
    auto core = logging::core::get();

    severity_by_channel.set_default(true);

    core->add_global_attribute("Scope", attrs::named_scope());
    core->add_global_attribute(
        "ThreadName", attrs::make_function(&thread::get_current_thread_name));
    set_logging_level(trace);

    ::atexit(&flush_logs);
}

void set_logging_level(LoggingLevel level)
{
    namespace phoenix = boost::phoenix;
    boost::log::core::get()->set_filter(
        phoenix::bind(&filter, phoenix::placeholders::_1, level));
}

void enable_console_logging()
{
    if (!console_logger)
    {
        console_logger =
            boost::log::add_console_log(std::cout, keywords::format = formatter);
        console_logger->set_filter(CommonppRecord.or_default(false) == true);
    }
}

void auto_flush_console(bool b)
{
    if (!console_logger)
    {
        return;
    }

    console_logger->locked_backend()->auto_flush(b);
}

void set_logging_level_for_channel(const std::string& channel, LoggingLevel level)
{
    severity_by_channel[channel] = level;
}

void enable_builtin_syslog()
{
#if HAVE_SYSLOG
    if (!syslog_logger)
    {
        using Sink = sinks::synchronous_sink<sinks::syslog_backend>;

        auto core = logging::core::get();
        boost::shared_ptr<sinks::syslog_backend> backend(
            new sinks::syslog_backend(keywords::facility = sinks::syslog::local0,
                                      keywords::use_impl = sinks::syslog::native));

        backend->set_severity_mapper(
            sinks::syslog::direct_severity_mapping<int>("LoggingLevel"));

        syslog_logger = boost::make_shared<Sink>(backend);
        syslog_logger->set_formatter(formatter);
        syslog_logger->set_filter(CommonppRecord.or_default(false) == true);

        core->add_sink(syslog_logger);
    }
#else
    ENABLE_CURRENT_FCT_LOGGING();
    GLOG(error) << "Cannot enable the syslog sink on this platform";
#endif
}

void add_file_sink(const std::string& path)
{
    boost::shared_ptr<sinks::text_file_backend> backend =
        boost::make_shared<sinks::text_file_backend>(keywords::file_name = path);

    auto core = logging::core::get();
    auto sink = boost::make_shared<FileSink>(backend);

    sink->set_filter(CommonppRecord.or_default(false) == true);
    core->add_sink(sink);

    sink->set_formatter(formatter);
}

void add_file_sink_rotate(const std::string& path,
                          size_t max_size,
                          boost::posix_time::hours period)
{
    boost::shared_ptr<sinks::text_file_backend> backend;

    if (max_size)
    {
        backend = boost::make_shared<sinks::text_file_backend>(
            keywords::file_name = path, keywords::rotation_size = max_size,
            keywords::time_based_rotation =
                sinks::file::rotation_at_time_interval(period));
    }
    else
    {
        backend = boost::make_shared<sinks::text_file_backend>(
            keywords::file_name = path,
            keywords::time_based_rotation =
                sinks::file::rotation_at_time_interval(period));
    }

    auto core = logging::core::get();
    auto sink = boost::make_shared<FileSink>(backend);
    sink->set_formatter(formatter);

    sink->set_filter(CommonppRecord.or_default(false) == true);
    core->add_sink(sink);
}

} // namespace core
} // namespace commonpp
