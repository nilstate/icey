///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/process.h"
#include <iostream>


namespace icy {


Process::Process(uv::Loop* loop)
    : _handle(loop)
    , _stdin(loop)
    , _stdout(loop)
    , _stderr(loop)
{
    init();
}


Process::Process(std::initializer_list<std::string> args, uv::Loop* loop)
    : args(args)
    , _handle(loop)
    , _stdin(loop)
    , _stdout(loop)
    , _stderr(loop)
{
    init();
}


Process::~Process()
{
    kill();
}


void Process::init()
{
    options.args = nullptr;
    options.env = nullptr;
    options.cwd = nullptr;
    options.flags = 0;
    options.stdio_count = 0;
    // options.uid = 0;
    // options.gid = 0;
    options.exit_cb = [](uv_process_t* req, int64_t exitStatus, int /*termSignal*/) {
        auto self = reinterpret_cast<Process*>(req->data);
        if (self->onexit)
            self->onexit(exitStatus);

        // Close the process handle to free resources.
        // After exit the handle is no longer active but must
        // still be closed to release the uv_process_t.
        self->_handle.close();
    };

    _stdin.init();
    _stdout.init();
    _stderr.init();

    _stdout.Read += [this](const char* data, const int& len) {
        if (onstdout)
            onstdout(std::string(data, len));
    };

    _stderr.Read += [this](const char* data, const int& len) {
        if (onstderr)
            onstderr(std::string(data, len));
    };

    options.stdio = _stdio;
    options.stdio[0].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_READABLE_PIPE);
    options.stdio[0].data.stream = _stdin.get<uv_stream_t>();
    options.stdio[1].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    options.stdio[1].data.stream = _stdout.get<uv_stream_t>();
    options.stdio[2].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    options.stdio[2].data.stream = _stderr.get<uv_stream_t>();
    options.stdio_count = 3;

    _handle.get()->data = this;
}


void Process::spawn()
{
    // Build c style args from STL containers.
    _cargs.clear();
    if (!args.empty()) {
        for (auto& arg : args)
            _cargs.push_back(&arg[0]);
        _cargs.push_back(nullptr);
    }

    if (!cwd.empty()) {
        options.cwd = cwd.c_str();
    }

    if (!file.empty()) {
        options.file = file.c_str();
        if (_cargs.empty()) {
            _cargs.push_back(&file[0]);
            _cargs.push_back(nullptr);
        }
    } else if (!_cargs.empty()) {
        options.file = _cargs[0];
    }

    if (_cargs.empty())
        throw std::runtime_error("Cannot spawn process: No file or args specified.");

    options.args = &_cargs[0];

    // Build c style env if specified.
    _cenv.clear();
    if (!env.empty()) {
        for (auto& e : env)
            _cenv.push_back(&e[0]);
        _cenv.push_back(nullptr);
        options.env = &_cenv[0];
    }

    // Spawn the process
    _handle.init(&uv_spawn, &options);
    _handle.throwLastError("Cannot spawn process");

    // Start reading on the stdout and stderr pipes
    if (!_stdout.readStart())
        _handle.setAndThrowError(_stdout.error().err, "Cannot read stdout pipe");
    if (!_stderr.readStart())
        _handle.setAndThrowError(_stderr.error().err, "Cannot read stderr pipe");
}


Pipe& Process::in()
{
    return _stdin;
}


Pipe& Process::out()
{
    return _stdout;
}


Pipe& Process::err()
{
    return _stderr;
}


bool Process::kill(int signum)
{
    if (!_handle.initialized() || _handle.closing() || _handle.closed())
        return false;
    int p = uv_process_get_pid(_handle.get());
    if (p <= 0)
        return false;
    return uv_process_kill(_handle.get(), signum) == 0;
}


int Process::pid() const
{
    if (!_handle.initialized())
        return 0;
    return uv_process_get_pid(_handle.get());
}


} // namespace icy


/// @}
