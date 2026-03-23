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
#include "icy/buffer.h"
#include "icy/handle.h"
#include "icy/logger.h"
#include "icy/request.h"
#include "icy/signal.h"

#include <stdexcept>


namespace icy {


/// Basic stream type for sockets and pipes.
template <typename T>
class Base_API Stream : public uv::Handle<T>
{
public:
    using Handle = uv::Handle<T>;

protected:
    struct OwnedWriteReq
    {
        uv_write_t req{};
        Buffer buffer;
    };

public:

    /// Construct the stream bound to @p loop with a 64 KiB read buffer.
    ///
    /// @param loop  Event loop to associate this stream with.
    Stream(uv::Loop* loop = uv::defaultLoop())
        : uv::Handle<T>(loop)
        , _buffer(65536)
    {
    }

    /// Destroy the stream, stopping reads and freeing pooled write requests.
    virtual ~Stream()
    {
        close();
        for (auto* req : _writeReqFree)
            delete req;
        for (auto* req : _ownedWriteReqFree)
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

    /// Send a TCP/pipe shutdown request to the connected peer.
    ///
    /// Issues a half-close: no further writes will be accepted after this, but
    /// the stream remains open for reading until the peer also closes.
    ///
    /// @return  `true` if the shutdown request was submitted successfully;
    ///          `false` if the stream is not active.
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

    /// Write @p len bytes from @p data to the stream.
    ///
    /// The write is non-blocking; data is buffered by libuv. Returns `false`
    /// without throwing if the stream is inactive, reads have not started, or
    /// the internal write queue exceeds the high-water mark.
    ///
    /// @param data  Pointer to the bytes to send. Must remain valid until the
    ///              write completion callback fires.
    /// @param len   Number of bytes to send.
    /// @return      `true` if the write was queued; `false` on backpressure or
    ///              if the stream is not in a writable state.
    bool write(const char* data, size_t len)
    {
        if (!canQueueWrite(len))
            return false;

        // Reuse write requests from freelist to avoid heap alloc per write
        uv_write_t* req = allocWriteReq();
        auto buf = uv_buf_init(const_cast<char*>(data), static_cast<unsigned int>(len));
        int err = uv_write(req, stream(), &buf, 1, [](uv_write_t* req, int) {
            // Return to freelist via the stream pointer stored in handle->data.
            // data is null if the C++ object was destroyed (Context::~Context
            // clears it before calling uv_close). In that case just free the req.
            auto self = reinterpret_cast<Stream*>(req->handle->data);
            if (self)
                self->freeWriteReq(req);
            else
                delete req;
        });
        if (err) {
            freeWriteReq(req);
            Handle::setUVError(err, "UV Error");
            return false;
        }
        return true;
    }

    /// Write an owned payload buffer to the stream.
    ///
    /// The buffer is moved into the queued write request and retained until the
    /// libuv completion callback fires. Use this path whenever the caller does
    /// not naturally own storage beyond the current stack frame.
    ///
    /// @param buffer  Payload buffer moved into the async write request.
    /// @return        `true` if the write was queued; `false` on backpressure or
    ///                if the stream is not in a writable state.
    bool writeOwned(Buffer&& buffer)
    {
        if (!canQueueWrite(buffer.size()))
            return false;

        auto* req = allocOwnedWriteReq();
        req->buffer = std::move(buffer);
        auto uvBuffer = uv_buf_init(req->buffer.data(), static_cast<unsigned int>(req->buffer.size()));
        int err = uv_write(&req->req, stream(), &uvBuffer, 1, [](uv_write_t* req, int) {
            auto* ownedReq = reinterpret_cast<OwnedWriteReq*>(req);
            auto* self = reinterpret_cast<Stream*>(req->handle->data);
            if (self)
                self->freeOwnedWriteReq(ownedReq);
            else
                delete ownedReq;
        });
        if (err) {
            freeOwnedWriteReq(req);
            Handle::setUVError(err, "UV Error");
            return false;
        }
        return true;
    }

    /// Set the high water mark for the write queue (default 16MB).
    /// When the write queue exceeds this size, write() returns false.
    void setHighWaterMark(size_t bytes) { _highWaterMark = bytes; }

    /// Write @p len bytes from @p data together with a stream handle over an
    /// IPC pipe (uses `uv_write2`).
    ///
    /// Only valid for named-pipe handles opened with IPC mode enabled.
    /// Throws `std::logic_error` if called on a non-IPC pipe.
    ///
    /// @param data  Bytes to send alongside the handle.
    /// @param len   Number of bytes to send.
    /// @param send  Stream handle to pass to the receiving process.
    /// @return      `true` if the write was queued; `false` on error.
    bool write(const char* data, size_t len, uv_stream_t* send)
    {
        if (!canQueueWrite(len))
            return false;

        if (stream()->type != UV_NAMED_PIPE || !this->template get<uv_pipe_t>()->ipc)
            throw std::logic_error("write2 is only valid for IPC pipes");

        auto buf = uv_buf_init(const_cast<char*>(data), static_cast<unsigned int>(len));
        auto* req = new uv_write_t;
        int err = uv_write2(req, stream(), &buf, 1, send, [](uv_write_t* req, int) {
            delete req;
        });
        if (err) {
            delete req;
            Handle::setUVError(err, "UV Error");
            return false;
        }
        return true;
    }

    /// Return the underlying `uv_stream_t` pointer cast from the native handle.
    ///
    /// @return  Pointer to the `uv_stream_t`, or `nullptr` if the handle is closed.
    uv_stream_t* stream()
    {
        return this->template get<uv_stream_t>();
    }

    /// Emitted when data has been received from the peer.
    ///
    /// Slot signature: `void(const char* data, const int& len)`
    Signal<void(const char*, const int&)> Read;

protected:
    /// Begin reading from the stream by registering libuv read callbacks.
    ///
    /// Sets the stream's `data` pointer to `this` so callbacks can recover the
    /// C++ object. Has no effect and returns `false` if already started.
    ///
    /// @return  `true` if `uv_read_start` was called successfully.
    virtual bool readStart()
    {
        // LTrace("Read start: ", ptr());
        if (_started)
            return false;
        _started = true;

        stream()->data = this;
        return Handle::invoke(&uv_read_start, stream(), Stream::allocReadBuffer, handleRead);
    }

    /// Stop reading from the stream.
    ///
    /// No further read callbacks will fire after this returns. Has no effect
    /// and returns `false` if not currently started.
    ///
    /// @return  `true` if `uv_read_stop` was called successfully.
    virtual bool readStop()
    {
        // LTrace("Read stop: ", ptr());
        if (!_started)
            return false;
        _started = false;

        return Handle::invoke(&uv_read_stop, stream());
    }

    /// Called by `handleRead` when @p len bytes of @p data arrive.
    ///
    /// The default implementation emits the `Read` signal. Override to intercept
    /// data before it reaches signal subscribers.
    ///
    /// @param data  Pointer into the read buffer; valid only for this call.
    /// @param len   Number of valid bytes in @p data.
    virtual void onRead(const char* data, size_t len)
    {
        // LTrace("On read: ", len);
        if (!Handle::initialized() || Handle::closed() || !_started)
            return;

        Read.emit(data, static_cast<int>(len));
    }

    //
    /// UV callbacks

    /// libuv read callback. Dispatches to `onRead()`, handles EOF by closing
    /// the stream gracefully, and propagates other errors via `setUVError()`.
    ///
    /// @param handle  The libuv stream that produced data.
    /// @param nread   Bytes read (>= 0), `UV_EOF`, or a negative error code.
    /// @param buf     Buffer containing the received data.
    static void handleRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
    {
        // LTrace("Handle read: ", nread);
        auto self = reinterpret_cast<Stream*>(handle->data);
#ifdef ICY_EXCEPTION_RECOVERY
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
#ifdef ICY_EXCEPTION_RECOVERY
        } catch (std::exception& exc) {
            // Exceptions thrown inside the read callback scope will set the
            // stream error in order to keep errors in the event loop
            LError("Stream exception: ", exc.what());
            self->setUVError(UV_UNKNOWN, exc.what());
        }
#endif
    }

    /// libuv allocate-buffer callback. Provides the stream's internal buffer,
    /// growing it if libuv requests more space than the current allocation.
    ///
    /// @param handle          The libuv handle requesting a buffer.
    /// @param suggested_size  Minimum size libuv would like for the buffer.
    /// @param buf             Output: filled with the buffer base and length.
    static void allocReadBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {
        auto& buffer = reinterpret_cast<Stream*>(handle->data)->_buffer;
        if (buffer.size() < suggested_size)
            buffer.resize(suggested_size);

        buf->base = buffer.data();
        buf->len = buffer.size();
    }

    /// Return a `uv_write_t` from the freelist, or allocate a new one if
    /// the pool is empty.
    ///
    /// @return  Pointer to an unused `uv_write_t`.
    uv_write_t* allocWriteReq()
    {
        if (!_writeReqFree.empty()) {
            auto* req = _writeReqFree.back();
            _writeReqFree.pop_back();
            return req;
        }
        return new uv_write_t;
    }

    /// Return @p req to the freelist, or delete it if the pool is at capacity.
    ///
    /// @param req  Write request to recycle or free.
    void freeWriteReq(uv_write_t* req)
    {
        if (_writeReqFree.size() < 8) { // cap pool size
            _writeReqFree.push_back(req);
        } else {
            delete req;
        }
    }

    OwnedWriteReq* allocOwnedWriteReq()
    {
        if (!_ownedWriteReqFree.empty()) {
            auto* req = _ownedWriteReqFree.back();
            _ownedWriteReqFree.pop_back();
            return req;
        }
        return new OwnedWriteReq;
    }

    void freeOwnedWriteReq(OwnedWriteReq* req)
    {
        req->buffer.clear();
        if (_ownedWriteReqFree.size() < 8) {
            _ownedWriteReqFree.push_back(req);
        } else {
            delete req;
        }
    }

protected:
    bool canQueueWrite(size_t len)
    {
        if (!Handle::active() || !_started)
            return false;

        size_t queueSize = stream()->write_queue_size;
        if (queueSize >= _highWaterMark || len > (_highWaterMark - queueSize)) {
            LWarn("Write queue full (", queueSize, " bytes), dropping write of ", len, " bytes");
            return false;
        }
        return true;
    }

    Buffer _buffer;
    bool _started{false};
    size_t _highWaterMark{16 * 1024 * 1024}; ///< 16MB default write queue limit
    std::vector<uv_write_t*> _writeReqFree;  ///< Freelist for write requests
    std::vector<OwnedWriteReq*> _ownedWriteReqFree; ///< Freelist for owned write requests
};


} // namespace icy


/// @\}
