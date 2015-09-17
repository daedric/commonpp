/*
 * File: examples/metrics/main.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include <commonpp/core/Utils.hpp>
#include <commonpp/metric/Metrics.hpp>
#include <commonpp/metric/sink/ConsoleSink.hpp>
#include <commonpp/metric/sink/GraphiteSink.hpp>
#include <commonpp/metric/sink/InfluxDBSink.hpp>
#include <commonpp/metric/type/TimeScope.hpp>
#include <commonpp/metric/reservoir/ExponentiallyDecayingReservoir.hpp>
#include "commonpp/core/LoggingInterface.hpp"

using commonpp::metric::Metrics;
using commonpp::metric::MetricTag;
using commonpp::metric::MetricValue;
using commonpp::metric::sink::ConsoleSink;
using commonpp::metric::sink::GraphiteSink;
using commonpp::metric::sink::InfluxDBSink;
using commonpp::thread::ThreadPool;

using commonpp::metric::reservoir::ExponentiallyDecayingReservoir;

using Counter = Metrics::Counter;
using DescStat = Metrics::DescStat;
using Gauge = Metrics::Gauge;
using TimeScope =
    commonpp::metric::type::TimeScope<ExponentiallyDecayingReservoir<>,
                                      std::chrono::seconds>;

int main(int, char *[])
{
    commonpp::core::init_logging();
    commonpp::core::enable_console_logging();
    commonpp::core::enable_builtin_syslog();

    DGLOG(warning) << "Hello world";

    ThreadPool pool(10, "", 5);
    pool.start();

    Metrics metrics(pool, std::chrono::seconds(1));

    MetricTag prefix;
    prefix += {"thisisa", "test"};

    ConsoleSink sink;
    GraphiteSink gsink(pool.getService(), prefix, "localhost", "2003");
    InfluxDBSink isink(pool.getService(), prefix, "commonpp_example",
                       "localhost", "8086");
    metrics.addMetricsReceiver(std::ref(sink));
    metrics.addMetricsReceiver(std::ref(gsink));
    metrics.addMetricsReceiver(std::ref(isink));

    MetricTag parent;
    parent += {"env", "dev"};
    parent += {"host", commonpp::get_hostname()};

    uintmax_t i = 0;
    metrics.add(parent("counter"), Counter(
                                       [&i]
                                       {
                                           return i;
                                       },
                                       "counter"));
    metrics.add(parent("gauge"), Gauge(
                                     [&i]
                                     {
                                         return i;
                                     },
                                     "gauge"));

    ExponentiallyDecayingReservoir<> reservoir;
    ExponentiallyDecayingReservoir<> timer_reservoir;
    metrics.add<DescStat>(parent("reservoir"), reservoir);
    metrics.add<DescStat>(parent("timer"), timer_reservoir);

    for (;;)
    {
        TimeScope scope(timer_reservoir);
        reservoir.pushValue(i);
        ++i;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
