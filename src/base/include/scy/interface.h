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

#include "uv.h" // ssize_t

#include <atomic>
#include <functional>
#include <memory>
#include <stdexcept>


namespace scy {


/// Interface classes
namespace basic {


class Base_API Decoder
{
public:
    Decoder() = default;
    virtual ~Decoder() = default;

    virtual ssize_t decode(const char* inbuf, size_t nread, char* outbuf) = 0;
    virtual ssize_t finalize(char* /* outbuf */) { return 0; }
};


class Base_API Encoder
{
public:
    Encoder() = default;
    virtual ~Encoder() = default;
    virtual ssize_t encode(const char* inbuf, size_t nread, char* outbuf) = 0;
    virtual ssize_t finalize(char* /* outbuf */) { return 0; }
};


/// Abstract interface for classes that can be run and cancelled.
class Base_API Runnable
{
public:
    Runnable()
        : exit(false)
    {
    }

    virtual ~Runnable() = default;

    /// The run method will be called by the asynchronous context.
    virtual void run() = 0;

    /// Cancel the current task.
    /// The run() method should return ASAP.
    virtual void cancel(bool flag = true)
    {
        exit = flag;
    }

    /// Returns true when the task has been cancelled.
    virtual bool cancelled() const
    {
        return exit.load();
    };

protected:
    std::atomic<bool> exit;
};


/// Abstract interface for a classes that can be started and stopped.
class Base_API Startable
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
};


/// Abstract interface for classes that can be sent and cancelled.
class Base_API Sendable
{
public:
    virtual bool send() = 0;
    virtual void cancel() = 0;
};


} // namespace basic
} // namespace scy


/// @\}
