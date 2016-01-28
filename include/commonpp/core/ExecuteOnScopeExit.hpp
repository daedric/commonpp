/*
 * File: ExecuteOnScopeExit.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once
#include <functional>

namespace commonpp
{

// compared to the boost one, this one can be cancelled.
struct ExecuteOnScopeExit final
{
    template <typename Callable>
    ExecuteOnScopeExit(Callable&& cb)
    : fn(std::forward<Callable>(cb))
    {
    }

    ExecuteOnScopeExit(const ExecuteOnScopeExit&) = delete;
    ExecuteOnScopeExit& operator=(const ExecuteOnScopeExit&) = delete;

    ~ExecuteOnScopeExit()
    {
        if (!cancelled)
        {
            fn();
        }
    }

    void cancel()
    {
        cancelled = true;
    }

    std::function<void()> fn;
    bool cancelled = false;
};

} // namespace commonpp
