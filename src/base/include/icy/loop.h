///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup uv UV module
///
/// The `uv` module contains C++ wrappers for `libuv`.
///
/// @addtogroup uv
/// @{


#pragma once


#include "icy/base.h"

#include "uv.h"


namespace icy {
namespace uv {


/// Alias for a `libuv` event loop instance.
using Loop = uv_loop_t;


/// Returns the process-wide default libuv event loop.
/// @return Pointer to the default `uv_loop_t`.
inline Loop* defaultLoop()
{
    return uv_default_loop();
}

/// Runs the given event loop using the specified run mode.
/// Blocks until the loop exits (when using `UV_RUN_DEFAULT`).
/// @param loop Event loop to run. Defaults to the default loop.
/// @param mode libuv run mode: `UV_RUN_DEFAULT`, `UV_RUN_ONCE`, or `UV_RUN_NOWAIT`.
inline void runLoop(Loop* loop = defaultLoop(), uv_run_mode mode = UV_RUN_DEFAULT)
{
    uv_run(loop, mode);
}

/// Stops the given event loop, causing `uv_run` to return after the current iteration.
/// @param loop Event loop to stop. Defaults to the default loop.
inline void stopLoop(Loop* loop = defaultLoop())
{
    uv_stop(loop);
}

/// Allocates and initializes a new libuv event loop.
/// The caller is responsible for closing and deleting the returned loop.
/// @return Pointer to a newly initialized `uv_loop_t`.
inline Loop* createLoop()
{
    auto loop = new uv_loop_t;
    uv_loop_init(loop);
    return loop;
}

/// Closes the given event loop, releasing internal resources.
/// All handles must be closed before calling this.
/// @param loop Event loop to close.
/// @return True on success, false if the loop still has active handles.
inline bool closeLoop(Loop* loop)
{
    return uv_loop_close(loop) == 0;
}


/// RAII wrapper for a libuv event loop.
/// Automatically closes and deletes the loop on destruction.
struct ScopedLoop
{
    Loop* loop;

    ScopedLoop()
        : loop(createLoop())
    {
    }

    ~ScopedLoop()
    {
        if (loop) {
            closeLoop(loop);
            delete loop;
        }
    }

    /// Implicit conversion to `Loop*` for use with libuv APIs.
    operator Loop*() const { return loop; }

    /// Returns the raw event loop pointer.
    /// @return Pointer to the underlying `uv_loop_t`.
    Loop* get() const { return loop; }

    ScopedLoop(const ScopedLoop&) = delete;
    ScopedLoop& operator=(const ScopedLoop&) = delete;
    ScopedLoop(ScopedLoop&&) = delete;
    ScopedLoop& operator=(ScopedLoop&&) = delete;
};


} // namespace uv
} // namespace icy


/// @}
