///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup uv
/// @{


#pragma once


#include "icy/base.h"
#include "icy/error.h"
#include "icy/loop.h"
#include "icy/memory.h"

#include "uv.h"

#include <memory>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>


namespace icy {
namespace uv {


template <typename T>
class Base_API Handle;

template <typename T>
struct Context;


template <typename T>
/// Extra storage placed around a raw `libuv` handle for close-time cleanup hooks.
struct HandleStorage
{
    T handle{};                               ///< Embedded raw `libuv` handle object.
    void* closeData = nullptr;                ///< Opaque cleanup payload invoked on close.
    void (*closeCleanup)(void*) = nullptr;    ///< Cleanup function for `closeData`.
};


template <typename T>
/// Returns the extended storage wrapper that owns @p handle.
/// @param handle Raw `libuv` handle pointer previously allocated by `Context<T>`.
inline HandleStorage<T>* handleStorage(T* handle)
{
    return reinterpret_cast<HandleStorage<T>*>(handle);
}


template <typename T>
/// Registers a cleanup callback that runs when @p handle finally closes.
/// @param handle Raw `libuv` handle pointer.
/// @param data User data passed back to @p cleanup.
/// @param cleanup Function invoked exactly once when the handle storage is released.
inline void setHandleCloseCleanup(T* handle, void* data, void (*cleanup)(void*))
{
    auto* storage = handleStorage(handle);
    storage->closeData = data;
    storage->closeCleanup = cleanup;
}


template <typename T>
/// Clears any pending close-time cleanup callback registered on @p handle.
/// @param handle Raw `libuv` handle pointer.
inline void clearHandleCloseCleanup(T* handle)
{
    auto* storage = handleStorage(handle);
    storage->closeData = nullptr;
    storage->closeCleanup = nullptr;
}


/// Shared `libuv` handle context.
template <typename T>
struct Context : public RefCounted<Context<T>>
{
    Handle<T>* handle = nullptr;
    HandleStorage<T>* storage = new HandleStorage<T>;
    T* ptr = &storage->handle;
    bool initialized = false;
    bool deleted = false;

    Context(Handle<T>* h)
        : handle(h)
    {
    }

    template <typename Owner>
    Owner* owner() const
    {
        static_assert(std::is_base_of_v<Handle<T>, Owner>,
            "Owner must derive from uv::Handle<T>");
        if (deleted || !handle)
            return nullptr;
        return static_cast<Owner*>(handle);
    }

    ~Context()
    {
        if (initialized) {
            auto* uvHandle = reinterpret_cast<uv_handle_t*>(ptr);
            uvHandle->data = nullptr;
            uv_close(uvHandle, [](uv_handle_t* handle) {
                auto* storage = reinterpret_cast<HandleStorage<T>*>(handle);
                if (storage->closeCleanup && storage->closeData)
                    storage->closeCleanup(storage->closeData);
                delete storage;
            });
        } else {
            if (storage->closeCleanup && storage->closeData)
                storage->closeCleanup(storage->closeData);
            delete storage;
        }
    }
};


/// Wrapper class for managing `uv_handle_t` variants.
///
/// This class manages the handle during its lifecycle and safely handles the
/// asynchronous destruction mechanism.
template <typename T>
class Base_API Handle
{
public:
    /// Construct the handle bound to the given event loop.
    ///
    /// @param loop  Event loop to associate this handle with. Defaults to the
    ///              process-wide default loop.
    Handle(uv::Loop* loop = uv::defaultLoop())
        : _loop(loop)
        , _context(makeIntrusive<Context<T>>(this))
    {
    }

    virtual ~Handle()
    {
    }

    /// Initialize the underlying libuv handle by calling @p f with the loop,
    /// the raw handle pointer, and any additional @p args.
    ///
    /// Must be called exactly once before any other operations.
    /// Throws `std::logic_error` if the handle is already initialized or the
    /// context is invalid.
    ///
    /// @param f     libuv init function (e.g. `uv_tcp_init`).
    /// @param args  Additional arguments forwarded after the loop and handle pointer.
    /// @return      `true` on success; `false` and sets the error state on failure.
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

    /// Invoke a libuv function @p f with @p args on the initialized handle.
    ///
    /// Throws `std::logic_error` if the handle is not yet initialized.
    /// Sets the error state and returns `false` if @p f returns a libuv error code.
    ///
    /// @param f     libuv function to call.
    /// @param args  Arguments forwarded to @p f.
    /// @return      `true` on success; `false` on libuv error.
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

    /// Invoke a libuv function @p f with @p args, throwing on failure.
    ///
    /// Identical to `invoke()` but throws a `std::runtime_error` with
    /// @p message prepended if @p f returns a libuv error code.
    /// Must not be called from inside a libuv callback.
    ///
    /// @param message  Error message prefix used in the thrown exception.
    /// @param f        libuv function to call.
    /// @param args     Arguments forwarded to @p f.
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
    ///
    /// Releases the `Context` (which schedules the async `uv_close`) and then
    /// fires `onClose()`. Safe to call more than once; subsequent calls are no-ops.
    virtual void close()
    {
        bool trigger = false;
        if (_context) {
            auto context = std::move(_context);
            context->deleted = true;
            context.reset();
            trigger = true;
        }
        if (trigger)
            onClose();
    }

    /// Re-reference the handle with the event loop after a previous `unref()`.
    ///
    /// When all handles are unref'd the loop exits automatically. This call
    /// reverses that. Has no effect if the handle is not initialized.
    void ref()
    {
        if (initialized())
            uv_ref(get<uv_handle_t>());
    }

    /// Unreference the handle from the event loop.
    ///
    /// The loop will exit when all active handles are unref'd, even if this
    /// handle is still alive. Has no effect if the handle is not initialized.
    void unref()
    {
        if (initialized())
            uv_unref(get<uv_handle_t>());
    }

    /// Return `true` if the handle has been successfully initialized via `init()`.
    bool initialized() const
    {
        return _context && _context->initialized;
    }

    /// Return `true` when the handle is active (libuv `uv_is_active`).
    ///
    /// "Active" has type-specific meaning: a timer is active while counting,
    /// a stream is active while connected, etc.
    virtual bool active() const
    {
        return get() && uv_is_active(get<uv_handle_t>()) != 0;
    }

    /// Return `true` if `uv_close` has been called and the handle is awaiting
    /// its close callback (libuv `uv_is_closing`).
    virtual bool closing() const
    {
        return get() && uv_is_closing(get<uv_handle_t>()) != 0;
    }

    /// Return `true` if the handle has been fully closed (context released).
    virtual bool closed() const
    {
        return !_context;
    }

    /// Return the last error set on this handle, or a default-constructed
    /// `Error` if no error has occurred.
    const icy::Error& error() const
    {
        return _error;
    }

    /// Set the error state and invoke `onError()`.
    ///
    /// @param error  Error value to store and propagate.
    virtual void setError(const Error& error)
    {
        // if (_error == err) return;
        assertThread();
        _error = error;
        onError(error);
    }

    /// Translate a libuv error code into an `Error` and call `setError()`.
    ///
    /// Safe to call from inside libuv callbacks.
    ///
    /// @param err     libuv error code (negative integer).
    /// @param prefix  Human-readable prefix prepended to the formatted message.
    void setUVError(int err, std::string prefix = "UV Error")
    {
        Error error;
        error.err = err;
        error.message = formatError(std::move(prefix), err);
        setError(error);
    }

    /// Set the error state from a libuv error code and throw a `std::runtime_error`.
    ///
    /// Must not be called from inside libuv callbacks; use `setUVError()` there.
    ///
    /// @param err     libuv error code (negative integer).
    /// @param prefix  Human-readable prefix prepended to the thrown message.
    void setAndThrowError(int err, std::string prefix = "UV Error")
    {
        setUVError(err, std::move(prefix));
        throwError(std::move(prefix), err);
    }

    /// Throw a `std::runtime_error` if the handle currently holds an error.
    ///
    /// The stored error's message is re-formatted with @p prefix before throwing.
    /// No-op if the handle is not in an error state.
    ///
    /// @param prefix  Human-readable prefix used when re-formatting the message.
    void throwLastError(std::string prefix = "UV Error")
    {
        if (error().any())
            setAndThrowError(error().err, std::move(prefix));
    }

    /// Return the event loop this handle is bound to.
    ///
    /// Asserts that the caller is on the owning thread.
    ///
    /// @return  Pointer to the associated `uv::Loop`.
    uv::Loop* loop() const
    {
        assertThread();
        return _loop;
    }

    /// Close the current handle (if open) and allocate a fresh `Context`,
    /// leaving the handle ready to be re-initialized via `init()`.
    void reset()
    {
        if (_context)
            close();
        _context = makeIntrusive<Context<T>>(this);
    }

    /// Return the raw libuv handle pointer cast to @p Handle.
    ///
    /// Returns `nullptr` if the context has been released (handle closed).
    /// Asserts that the caller is on the owning thread.
    ///
    /// @tparam Handle  Target type; defaults to the native handle type `T`.
    /// @return         Pointer to the underlying libuv handle, or `nullptr`.
    template <typename Handle = T>
    Handle* get() const
    {
        assertThread();
        return _context ? reinterpret_cast<Handle*>(_context->ptr) : nullptr;
    }

    /// Return the ID of the thread that constructed this handle.
    ///
    /// All handle operations must be performed on this thread.
    ///
    /// @return  `std::thread::id` of the owning thread.
    std::thread::id tid() const
    {
        return _tid;
    }

    /// Return the raw `Context` that owns the libuv handle memory.
    ///
    /// Primarily for use by subclasses and libuv callbacks that need to access
    /// the underlying libuv handle memory.
    ///
    /// @return  A retained reference to the `Context`, or an empty reference if closed.
    IntrusivePtr<Context<T>> context() const
    {
        return _context;
    }

    template <typename U>
    void setCloseCleanup(U* data)
    {
        if (!_context)
            throw std::logic_error("Handle not initialized");
        uv::setHandleCloseCleanup(get(), data, [](void* ptr) {
            delete static_cast<U*>(ptr);
        });
    }

    void clearCloseCleanup()
    {
        if (_context)
            uv::clearHandleCloseCleanup(get());
    }

    /// Throw `std::logic_error` if called from any thread other than the
    /// thread that constructed this handle.
    void assertThread() const
    {
        if (std::this_thread::get_id() != _tid) {
            throw std::logic_error("Handle accessed from wrong thread");
        }
    }

    /// Define the native handle type.
    using Type = T;

protected:
    /// Called by `setError()` after the error state has been updated.
    ///
    /// Override to react to errors. The default implementation is a no-op.
    ///
    /// @param error  The error that was set.
    virtual void onError(const Error& error)
    {
    }

    /// Called by `close()` after the context has been released.
    ///
    /// Override to perform cleanup on handle closure. The default implementation
    /// is a no-op.
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
    IntrusivePtr<Context<T>> _context;
    std::thread::id _tid = std::this_thread::get_id();
    Error _error;
};


template <typename Owner, typename Callback>
/// Wraps @p callback so it only runs while the owning handle is still alive.
/// Captures the intrusive `Context` token, rehydrates the typed owner on entry,
/// and suppresses invocation if the handle has already been deleted.
/// @param owner Owning handle instance.
/// @param callback Callable that receives `Owner&` followed by the libuv callback args.
inline auto withHandleContext(Owner& owner, Callback&& callback)
{
    using OwnerType = std::remove_reference_t<Owner>;
    using CallbackType = std::decay_t<Callback>;
    auto context = owner.context();
    return [context = std::move(context),
            callback = CallbackType(std::forward<Callback>(callback))](
               auto&&... args) mutable {
        if (context) {
            if (auto* live = context->template owner<OwnerType>()) {
                callback(*live, std::forward<decltype(args)>(args)...);
            }
        }
    };
}


} // namespace uv
} // namespace icy


/// @}
