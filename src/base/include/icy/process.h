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
#include "icy/handle.h"
#include "icy/pipe.h"
#include <csignal>
#include <functional>
#include <initializer_list>
#include <vector>


namespace icy {


/// Raw `libuv` process spawn options passed through to `uv_spawn`.
using ProcessOptions = uv_process_options_t;


/// Spawns and manages a child process with stdin/stdout/stderr pipes
class Base_API Process
{
public:
    /// Constructs a `Process` attached to the given event loop.
    /// @param loop Event loop to use for I/O and exit notifications. Defaults to the default loop.
    Process(uv::Loop* loop = uv::defaultLoop());

    /// Constructs a `Process` with initial command-line arguments.
    /// @param args Initializer list of argument strings. The first element is typically the executable path.
    /// @param loop Event loop to use for I/O and exit notifications. Defaults to the default loop.
    Process(std::initializer_list<std::string> args, uv::Loop* loop = uv::defaultLoop());

    /// Destructor.
    ~Process();

    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    Process(Process&&) = delete;
    Process& operator=(Process&&) = delete;

    /// Path to the program to execute.
    /// Convenience proxy for options.file.
    /// Must be set before `spawn()`
    std::string file;

    /// Set the current working directory.
    /// Convenience proxy for options.cwd.
    /// Must be set before `spawn()`
    std::string cwd;

    /// Command line arguments to pass to the process.
    /// Convenience proxy for options.args.
    /// Must be set before `spawn()`
    std::vector<std::string> args;

    /// Environment variables for the process.
    /// Each entry should be in "KEY=VALUE" format.
    /// If empty, the child inherits the parent environment.
    /// Must be set before `spawn()`
    std::vector<std::string> env;

    /// Spawns the process.
    /// Options must be properly set.
    /// Throws an exception on error.
    void spawn();

    /// Sends a signal to the process.
    /// @param signum Signal number to send (default: `SIGTERM`).
    /// @return True if the signal was sent successfully, false if the process is not running or handle is invalid.
    bool kill(int signum = SIGTERM);

    /// Returns the process PID, or 0 if not spawned.
    int pid() const;

    /// Returns the stdin pipe.
    Pipe& in();

    /// Returns the stdout pipe.
    Pipe& out();

    /// Returns the stderr pipe.
    Pipe& err();

    /// Stdout callback.
    /// Called when a line has been output from the process.
    std::function<void(std::string)> onstdout;

    /// Stderr callback.
    /// Called when a line has been output on stderr.
    std::function<void(std::string)> onstderr;

    /// Exit callback.
    /// Called with process exit status code.
    std::function<void(std::int64_t)> onexit;

    /// LibUV C options.
    /// Available for advanced use cases.
    ProcessOptions options;

protected:
    void init();

    uv::Handle<uv_process_t> _handle;
    Pipe _stdin;
    Pipe _stdout;
    Pipe _stderr;
    uv_stdio_container_t _stdio[3];
    std::vector<char*> _cargs;
    std::vector<char*> _cenv;
};


} // namespace icy


/// @}
