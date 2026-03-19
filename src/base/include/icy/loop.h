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


using Loop = uv_loop_t;


inline Loop* defaultLoop()
{
    return uv_default_loop();
}

inline void runLoop(Loop* loop = defaultLoop(), uv_run_mode mode = UV_RUN_DEFAULT)
{
    uv_run(loop, mode);
}

inline void stopLoop(Loop* loop = defaultLoop())
{
    uv_stop(loop);
}

inline Loop* createLoop()
{
    auto loop = new uv_loop_t;
    uv_loop_init(loop);
    return loop;
}

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

    operator Loop*() const { return loop; }
    Loop* get() const { return loop; }

    ScopedLoop(const ScopedLoop&) = delete;
    ScopedLoop& operator=(const ScopedLoop&) = delete;
    ScopedLoop(ScopedLoop&&) = delete;
    ScopedLoop& operator=(ScopedLoop&&) = delete;
};


} // namespace uv
} // namespace icy


/// @\}
