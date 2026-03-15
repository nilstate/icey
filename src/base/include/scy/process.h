///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/handle.h"
#include "scy/pipe.h"
#include <functional>
#include <initializer_list>
#include <vector>


namespace scy {


using ProcessOptions = uv_process_options_t;


class Base_API Process
{
public:
    /// Default constructor.
    Process(uv::Loop* loop = uv::defaultLoop());

    /// Constructor with command line arguments.
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

    /// Kills the process.
    bool kill(int signum = SIGKILL);

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


} // namespace scy


/// @\}
