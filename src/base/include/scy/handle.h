///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup uv
/// @{


#pragma once


#include "scy/base.h"
#include "scy/error.h"
#include "scy/loop.h"

#include "uv.h"

#include <memory>
#include <stdexcept>
#include <thread>


namespace scy {
namespace uv {


template <typename T>
class Base_API Handle;


/// Shared `libuv` handle context.
template <typename T>
struct Context
{
    Handle<T>* handle = nullptr;
    T* ptr = new T;
    bool initialized = false;
    bool deleted = false;

    Context(Handle<T>* h)
        : handle(h)
    {
    }

    ~Context()
    {
        if (initialized) {
            uv_close(reinterpret_cast<uv_handle_t*>(ptr), [](uv_handle_t* handle) {
                delete handle;
            });
        } else {
            delete ptr;
        }
    }
};


/// Wrapper class for managing `uv_handle_t` variants.
///
/// This class manages the handle during it's lifecycle and safely handles the
/// asynchronous destruction mechanism.
template <typename T>
class Base_API Handle
{
public:
    Handle(uv::Loop* loop = uv::defaultLoop())
        : _loop(loop)
        , _context(std::make_unique<Context<T>>(this))
    {
    }

    virtual ~Handle()
    {
    }

    /// Initialize the handle.
    template <typename F, typename... Args>
    bool init(F&& f, Args&&... args)
    {
        assertThread();
        if (!_context || initialized())
            throw std::logic_error("Handle not in valid state for initialization");
        int err = std::forward<F>(f)(loop(), get(), std::forward<Args>(args)...);
        if (err)
            setUVError(err, "Initialization failed");
        else
            _context->initialized = true;
        return !err;
    }

    /// Invoke a method on the handle.
    template <typename F, typename... Args>
    bool invoke(F&& f, Args&&... args) //, const std::string& prefix = "UV Error")
    {
        assertThread();
        if (!initialized())
            throw std::logic_error("Handle not initialized");
        int err = std::forward<F>(f)(std::forward<Args>(args)...);
        if (err)
            setUVError(err, "UV Error");
        return !err;
    }

    /// Invoke a method on the handle.
    ///
    /// An exception will be thrown if the invoked method returns an error.
    template <typename F, typename... Args>
    void invokeOrThrow(const std::string& message, F&& f, Args&&... args)
    {
        assertThread();
        if (!initialized())
            throw std::logic_error("Handle not initialized");
        int err = std::forward<F>(f)(std::forward<Args>(args)...);
        if (err)
            setAndThrowError(message, err);
    }

    /// Close and destroy the handle.
    virtual void close()
    {
        bool trigger = false;
        if (_context) {
            _context->deleted = true;
            trigger = true;
        }
        _context.reset();
        if (trigger)
            onClose();
    }

    /// Reference main loop again, once unref'd.
    void ref()
    {
        if (initialized())
            uv_ref(get<uv_handle_t>());
    }

    /// Unreference the main loop after initialized.
    void unref()
    {
        if (initialized())
            uv_unref(get<uv_handle_t>());
    }

    /// Return true if the handle has been initialized.
    bool initialized() const
    {
        return _context && _context->initialized;
    }

    /// Return true when the handle is active.
    virtual bool active() const
    {
        return get() && uv_is_active(get<uv_handle_t>()) != 0;
    }

    /// Return true if the handle is closing.
    virtual bool closing() const
    {
        return get() && uv_is_closing(get<uv_handle_t>()) != 0;
    }

    /// Return true is the handle has been closed.
    virtual bool closed() const
    {
        return !_context;
    }

    /// Return the error context if any.
    const scy::Error& error() const
    {
        return _error;
    }

    /// Set the error and triggers callbacks.
    virtual void setError(const Error& error)
    {
        // if (_error == err) return;
        assertThread();
        _error = error;
        onError(error);
    }

    /// Set the error and trigger relevant callbacks.
    /// This method can be called inside `libuv` callbacks.
    void setUVError(int err, std::string prefix = "UV Error")
    {
        Error error;
        error.err = err;
        error.message = formatError(std::move(prefix), err);
        setError(error);
    }

    /// Set the error and throw an exception.
    /// Should never be called inside `libuv` callbacks.
    void setAndThrowError(int err, std::string prefix = "UV Error")
    {
        setUVError(err, std::move(prefix));
        throwError(std::move(prefix), err);
    }

    /// Throw an exception if the handle is in error state.
    /// The error message prefix will be updated if provided.
    void throwLastError(std::string prefix = "UV Error")
    {
        if (error().any())
            setAndThrowError(error().err, std::move(prefix));
    }

    /// Return a cast pointer to the managed `libuv` handle.
    uv::Loop* loop() const
    {
        assertThread();
        return _loop;
    }

    /// Reset the internal handle pointer and container state.
    void reset()
    {
        if (_context)
            close();
        _context = std::make_unique<Context<T>>(this);
    }

    /// Return a typecasted pointer to the managed handle.
    template <typename Handle = T>
    Handle* get() const
    {
        assertThread();
        return _context ? reinterpret_cast<Handle*>(_context->ptr) : nullptr;
    }

    /// Return the parent thread ID.
    std::thread::id tid() const
    {
        return _tid;
    }

    /// Return the handle pointer context.
    Context<T>* context() const
    {
        return _context.get();
    }

    /// Assert the call is from the parent event loop thread.
    void assertThread() const
    {
        if (std::this_thread::get_id() != _tid) {
            throw std::logic_error("Handle accessed from wrong thread");
        }
    }

    /// Define the native handle type.
    using Type = T;

protected:
    /// Error callback.
    /// Override to handle errors.
    /// The error may be a UV error, or a custom error.
    virtual void onError(const Error& error)
    {
    }

    /// Close callback.
    /// Override to handle closure.
    virtual void onClose()
    {
    }

protected:
    /// NonCopyable and NonMovable
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    Handle(Handle&&) = delete;
    Handle& operator=(Handle&&) = delete;

    uv::Loop* _loop;
    std::unique_ptr<Context<T>> _context;
    std::thread::id _tid = std::this_thread::get_id();
    Error _error;
};


} // namespace uv
} // namespace scy


/// @\}
