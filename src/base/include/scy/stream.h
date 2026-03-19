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
#include "scy/buffer.h"
#include "scy/handle.h"
#include "scy/logger.h"
#include "scy/request.h"
#include "scy/signal.h"

#include <stdexcept>


namespace scy {


/// Basic stream type for sockets and pipes.
template <typename T>
class Base_API Stream : public uv::Handle<T>
{
public:
    using Handle = uv::Handle<T>;

    Stream(uv::Loop* loop = uv::defaultLoop())
        : uv::Handle<T>(loop)
        , _buffer(65536)
    {
    }

    virtual ~Stream()
    {
        close();
        for (auto* req : _writeReqFree)
            delete req;
    }

    /// Closes and resets the stream handle.
    /// This will close the active socket/pipe and destroy the handle.
    ///
    /// If the stream is already closed this call will have no side-effects.
    virtual void close() override
    {
        // LTrace("Close: ", ptr());
        if (_started)
            readStop();
        Handle::close();
    }

    /// Sends a shutdown packet to the connected peer.
    /// Return true if the shutdown packet was sent.
    bool shutdown()
    {
        Handle::assertThread();
        if (!Handle::active())
            return false;

        return uv_shutdown(new uv_shutdown_t, stream(),
                           [](uv_shutdown_t* req, int) {
                               delete req;
                           }) == 0;
    }

    /// Writes data to the stream.
    ///
    /// Return false if the underlying socket is closed or if the write
    /// queue has exceeded the high water mark (backpressure).
    /// This method does not throw an exception.
    bool write(const char* data, size_t len)
    {
        if (!Handle::active() || !_started)
            return false;

        // Backpressure: reject writes if libuv's write queue is too large.
        // This prevents unbounded memory growth on slow connections.
        size_t queueSize = stream()->write_queue_size;
        if (queueSize > _highWaterMark) {
            LWarn("Write queue full (", queueSize, " bytes), dropping write of ", len, " bytes");
            return false;
        }

        // Reuse write requests from freelist to avoid heap alloc per write
        uv_write_t* req = allocWriteReq();
        auto buf = uv_buf_init(const_cast<char*>(data), static_cast<unsigned int>(len));
        return Handle::invoke(&uv_write, req, stream(), &buf, 1, [](uv_write_t* req, int) {
            // Return to freelist via the stream pointer stored in handle->data.
            // data is null if the C++ object was destroyed (Context::~Context
            // clears it before calling uv_close). In that case just free the req.
            auto self = reinterpret_cast<Stream*>(req->handle->data);
            if (self)
                self->freeWriteReq(req);
            else
                delete req;
        });
    }

    /// Set the high water mark for the write queue (default 16MB).
    /// When the write queue exceeds this size, write() returns false.
    void setHighWaterMark(size_t bytes) { _highWaterMark = bytes; }

    /// Write data to the target stream.
    ///
    /// This method is only valid for IPC streams.
    bool write(const char* data, size_t len, uv_stream_t* send)
    {
        if (!Handle::active() || !_started)
            return false;

        if (stream()->type != UV_NAMED_PIPE || !this->template get<uv_pipe_t>()->ipc)
            throw std::logic_error("write2 is only valid for IPC pipes");

        auto buf = uv_buf_init(const_cast<char*>(data), static_cast<unsigned int>(len));
        return Handle::invoke(&uv_write2, new uv_write_t, stream(), &buf, 1, send, [](uv_write_t* req, int) {
            delete req;
        });
    }

    /// Return the uv_stream_t pointer.
    uv_stream_t* stream()
    {
        return this->template get<uv_stream_t>();
    }

    /// Signal the notifies when data is available for read.
    Signal<void(const char*, const int&)> Read;

protected:
    virtual bool readStart()
    {
        // LTrace("Read start: ", ptr());
        if (_started)
            return false;
        _started = true;

        stream()->data = this;
        return Handle::invoke(&uv_read_start, stream(), Stream::allocReadBuffer, handleRead);
    }

    virtual bool readStop()
    {
        // LTrace("Read stop: ", ptr());
        if (!_started)
            return false;
        _started = false;

        return Handle::invoke(&uv_read_stop, stream());
    }

    virtual void onRead(const char* data, size_t len)
    {
        // LTrace("On read: ", len);
        if (!Handle::initialized() || Handle::closed() || !_started)
            return;

        Read.emit(data, static_cast<int>(len));
    }

    //
    /// UV callbacks

    static void handleRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
    {
        // LTrace("Handle read: ", nread);
        auto self = reinterpret_cast<Stream*>(handle->data);
#ifdef SCY_EXCEPTION_RECOVERY
        try {
#endif
            if (nread >= 0) {
                self->onRead(buf->base, nread);
            } else if (nread == UV_EOF) {
                // EOF is not an error - it's the normal result of the peer
                // closing their end of the connection (e.g. after shutdown).
                // Close the stream gracefully instead of setting error state.
                self->close();
            } else {
                self->setUVError(static_cast<int>(nread), "Stream read error");
            }
#ifdef SCY_EXCEPTION_RECOVERY
        } catch (std::exception& exc) {
            // Exceptions thrown inside the read callback scope will set the
            // stream error in order to keep errors in the event loop
            LError("Stream exception: ", exc.what());
            self->setUVError(UV_UNKNOWN, exc.what());
        }
#endif
    }

    static void allocReadBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {
        auto& buffer = reinterpret_cast<Stream*>(handle->data)->_buffer;
        if (buffer.size() < suggested_size)
            buffer.resize(suggested_size);

        buf->base = buffer.data();
        buf->len = buffer.size();
    }

    uv_write_t* allocWriteReq()
    {
        if (!_writeReqFree.empty()) {
            auto* req = _writeReqFree.back();
            _writeReqFree.pop_back();
            return req;
        }
        return new uv_write_t;
    }

    void freeWriteReq(uv_write_t* req)
    {
        if (_writeReqFree.size() < 8) { // cap pool size
            _writeReqFree.push_back(req);
        } else {
            delete req;
        }
    }

protected:
    Buffer _buffer;
    bool _started{false};
    size_t _highWaterMark{16 * 1024 * 1024}; ///< 16MB default write queue limit
    std::vector<uv_write_t*> _writeReqFree;  ///< Freelist for write requests
};


} // namespace scy


/// @\}
