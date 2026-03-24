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

#include "uv.h" // ssize_t

#include <atomic>
#include <functional>
#include <memory>
#include <stdexcept>


namespace icy {


/// Interface classes
namespace basic {


/// Abstract interface for stream decoders
class Base_API Decoder
{
public:
    Decoder() = default;
    virtual ~Decoder() = default;

    /// Decodes nread bytes from inbuf and writes decoded output to outbuf.
    /// @param inbuf  Encoded input buffer.
    /// @param nread  Number of bytes to decode from inbuf.
    /// @param outbuf Destination buffer for decoded output.
    /// @return Number of bytes written to outbuf, or -1 on error.
    virtual ssize_t decode(const char* inbuf, size_t nread, char* outbuf) = 0;

    /// Flushes any buffered state and writes final output to outbuf.
    /// @param outbuf Destination buffer for any remaining output.
    /// @return Number of bytes written, or 0 if nothing to flush.
    virtual ssize_t finalize(char* outbuf) { (void)outbuf; return 0; }
};


/// Abstract interface for stream encoders
class Base_API Encoder
{
public:
    Encoder() = default;
    virtual ~Encoder() = default;

    /// Encodes nread bytes from inbuf and writes encoded output to outbuf.
    /// @param inbuf  Raw input buffer to encode.
    /// @param nread  Number of bytes to encode from inbuf.
    /// @param outbuf Destination buffer for encoded output.
    /// @return Number of bytes written to outbuf, or -1 on error.
    virtual ssize_t encode(const char* inbuf, size_t nread, char* outbuf) = 0;

    /// Flushes any buffered state and writes final output to outbuf.
    /// @param outbuf Destination buffer for any remaining output.
    /// @return Number of bytes written, or 0 if nothing to flush.
    virtual ssize_t finalize(char* outbuf) { (void)outbuf; return 0; }
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
    /// Starts the object (e.g. begins processing or listening).
    virtual void start() = 0;

    /// Stops the object (e.g. halts processing or closes resources).
    virtual void stop() = 0;
};


/// Abstract interface for classes that can be sent and cancelled.
class Base_API Sendable
{
public:
    /// Initiates the send operation.
    /// @return true if the send was dispatched successfully, false otherwise.
    virtual bool send() = 0;

    /// Cancels a pending send operation.
    virtual void cancel() = 0;
};


} // namespace basic
} // namespace icy


/// @}
