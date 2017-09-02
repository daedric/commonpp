[![Build status](https://ci.appveyor.com/api/projects/status/uicpe9aefv7ufiqs/branch/master?svg=true)](https://ci.appveyor.com/project/daedric/commonpp/branch/master)

# CommonPP

`commonpp` is a multi purpose library easing very few operations like:

* Getting metrics out of your program (counter, gauge, statistical description
  of sample);
* Naming your threads (and have the custom name appearing in `htop`, `top`,
  `gdb`);
* Have a really simple thread pool on top of `boost::asio`;
* Quick setup of `boost::log`;
* …

### License

I chose the BSD 2-clause license, so the library can be used in any project,
however I'd appreciate any bug fix, pull request and comment.

### Dependencies

`commonpp` depends on Boost and TBB, a C++11 compiler and CMake to be built.
It builds on Mac OS X and Linux. It probably build on *BSD but I did not test
(yet).

### Building

    $> make cmake
    $> make
    $> make test

#### On windows

`commonpp` can be built and used on windows. The prefered way is to use
`vcpkg`.

The required dependencies can be installed using the following command:

    $> .\vcpkg.exe install boost:x64-windows tbb:x64-windows hwloc:x64-windows

Then you can generate a Visual Studio solution giving the vcpkg toolchain file.

Please note that only VS2017 has been tested. Also, static `TBB` installation
cannot be done, so the DLL will need to be in the path to execute binaries
depending on `commonpp`.

### Why?

Although Boost and TBB are very complete low-level libraries, there are still
basic features missing, the biggest being getting out metrics out of your code.
`commonpp` will never replace Boost or TBB but instead adds some features that
integrate well.

## Libraries

### Core

The core part is mostly utility functions or class.

* `ExecuteOnScopeExit`: executes a callable on scope exit. It can be canceled;
* `LoggingInterface`: It is a header containing several logging function on top
  of `boost::log`. Every record produced with `commonpp` is tagged so that it
  can be used in a project already using `boost::log` (therefore `init_logging`
  should not be called).
* `RandomValuePicker`: select a random value in a read only container;
* `FloatingArithmeticTools`: Knuth's double comparison functions;
* `Options`: an utility class working along with an enum to offer a simple
  interface to manage options, see [the test](tests/core/options.cpp);
* There are several string functions to stringify, encode, join, or get a
  formatted date.

### Thread

The thread library is quite small:

* `Thread.hpp` contains a function to get/set the current thread name;
* `ThreadPool` is a class managing several threads calling the
  `boost::asio::io_service::run` member function:

      * It can schedule a callable to be called periodically;
      * It supports several `io_service`;
      * A function can be called on thread startup to setup any thread specific
        data;

* `Spinlock`: should be obvious
* `ThreadTimer`: it allows one to get the load of the current thread. This is
  experimental;

### Metrics

This is the main reason this `commonpp` exists. I often found myself struggling
to get metrics out of my code to monitor performances, behavior, resource
usage, etc. Most of the library counters are based on the RRD ones, the
`ExponentiallyDecayingReservoir` is based on the implementation in Codahale as
well as the test.

* `Metrics.hpp` is the entry point of the metric library; one should use it to
  register counters;
* `MetricTag`: This represents the metadata associated with a counter; for
  instance if you use Graphite, the tag will be converted in a path
  (concatenation of all the value separated by a point), if you use InfluxDB,
  it will be converted in a list of tag;
* `MetricValue`: This holds the actual value(s) of a counter.

#### The counters

There are 3 types of counter:

* `Counter` or `SharedCounter` should be used to represent a value that always
  increases and never goes back like a request counter. The actual value sent
  is the rate at which the counter increases. The SharedCounter can be shared
  among several thread safely.
* `Gauge`: It represents an absolute value like a number of active connections.
* `DescStat`: Associated with a reservoir, it will describe the distribution of
  the values, for instance, one can use a reservoir to push the time a function
  `X` takes to run, and the `StatDesc` will give several information like the
  min, max, variance, quantile, mean, etc.

There is also a helper:

* `TimeScope` is an helper to measure the time we spent in the current scope.


#### Examples

There is a complete example how to use the metric library
[here](examples/metrics/main.cpp)

#### The reservoirs

Currently only one is implemented: `ExponentiallyDecayingReservoir`.
Documentation can be found
[there](https://github.com/dropwizard/metrics/blob/master/metrics-core/src/main/java/io/dropwizard/metrics/ExponentiallyDecayingReservoir.java).

#### The sinks

* Graphite: It sends a list of `MetricTag` and `MetricValue` pair in a format
  understood by Graphite (actually Carbon). As stated before, the values are
  concatenated to form the metric path, therefore the order used to insert the
  tag is important.
* InfluxDB: It sends the measure name along with all the tags and the value
  as described in the documentation. The `MetricTag` and `MetricValue` class
  were designed with InfluxDB primarely in mind.

### Net

This library for now consist in a HTTP request generator and parser and URL
encode and decode functions.


## Next step

* Migrate my other project HTTPP to use commonpp;
* Add tests.
