/*
 * File: bench/shared_counter/main.cpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */

#include <iostream>
#include <commonpp/metric/type/Counter.hpp>
#include <commonpp/thread/ThreadPool.hpp>

using commonpp::metric::type::SharedCounter;
using commonpp::thread::ThreadPool;
using Clock = std::chrono::high_resolution_clock;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "USAGE:\n\t" << argv[0] << " thread iteration" << std::endl;
        return EXIT_FAILURE;
    }

    int nb_thread = std::stoi(argv[1]);
    static const size_t ITERATION = std::stoi(argv[2]);

    ThreadPool thread_pool(nb_thread);
    thread_pool.start();

    SharedCounter counter;
    auto begin = Clock::now();
    std::atomic_int thread_running(0);
    for (int i = 0; i < nb_thread; ++i)
    {
        thread_pool.post([&counter, &thread_running]
                         {
                             size_t i = 0;
                             thread_running++;
                             for (; i < ITERATION; ++i)
                                 counter++;
                         });
    }

    while (thread_running.load() != nb_thread)
        ;
    thread_pool.stop();
    auto end = Clock::now();

    std::cout << "Rate: "
              << (ITERATION * nb_thread) /
                     (std::chrono::duration_cast<std::chrono::seconds>(end - begin)
                          .count()) << "i/s";

    return 0;
}

// With 8th * 100M (RelWithDebInfo)
// TBB: 400M/s
// Atomic: 33M/s
// Spinlock (TBB): 14M/s
// Spinlock (Atomic): < 2M/s
