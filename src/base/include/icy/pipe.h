///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/stream.h"


namespace icy {


/// Named pipe / stdio stream built on `uv_pipe_t`.
///
/// Suitable for inter-process communication and for wrapping process stdio
/// (stdin/stdout/stderr). IPC mode allows passing stream handles between
/// processes over the pipe.
class Base_API Pipe : public Stream<uv_pipe_t>
{
public:
    /// Construct a `Pipe` bound to @p loop without initializing the libuv handle.
    ///
    /// Call `init()` before performing any I/O.
    ///
    /// @param loop  Event loop to associate with. Defaults to the process-wide default loop.
    Pipe(uv::Loop* loop = uv::defaultLoop());

    /// Destroy the pipe, stopping reads and closing the handle.
    virtual ~Pipe();

    /// Initialize the underlying `uv_pipe_t` handle.
    ///
    /// Must be called before `readStart()` or any write operations.
    ///
    /// @param ipc  Set to `true` to enable IPC mode, which allows sending and
    ///             receiving stream handles alongside data via `write()`.
    virtual void init(bool ipc = false);

    /// Start reading from the pipe.
    ///
    /// Delegates to `Stream<uv_pipe_t>::readStart()`. Emits the `Read` signal
    /// as data arrives.
    ///
    /// @return  `true` if `uv_read_start` was called successfully.
    bool readStart() override;
    //virtual bool readStop() override;
};


} // namespace icy


/// @\}
