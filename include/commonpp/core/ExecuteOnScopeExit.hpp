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
template <typename Callable>
struct ExecuteOnScopeExit final
{
    ExecuteOnScopeExit(Callable&& cb)
    : fn(std::forward<Callable>(cb))
    {
    }

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

    Callable fn;
    bool cancelled = false;
};

template <typename Callable>
ExecuteOnScopeExit<Callable> execute_on_scope_exit(Callable&& callable)
{
    return {std::forward<Callable>(callable)};
}

} // namespace commonpp
