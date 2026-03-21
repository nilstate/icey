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
#include "icy/interface.h"
#include "icy/logger.h"
#include "icy/platform.h"

#include <deque>
#include <stdexcept>


namespace icy {


//
// Synchronization Context
//

/// Synchronizer enables any thread to communicate with
/// the associated event loop via synchronized callbacks.
///
/// This class inherits the `Runner` interface and may be used with any
/// implementation that's powered by an asynchronous `Runner`.
///
class Base_API Synchronizer : public Runner
{
public:
    // using uv::Handle<uv_async_t>;

    /// Creates a synchronizer attached to the given event loop without a callback.
    /// Call `start()` separately to register the callback before using `post()`.
    /// @param loop Event loop to attach the async handle to.
    Synchronizer(uv::Loop* loop);

    /// Creates and immediately starts a synchronizer with a single callback function.
    /// The target is invoked from the event loop context each time `post()` is called.
    /// @param target Callback to invoke on each event loop wakeup.
    /// @param loop Event loop to attach the async handle to.
    Synchronizer(std::function<void()> target, uv::Loop* loop = uv::defaultLoop());

    /// Creates and immediately starts a synchronizer with a variadic callback.
    /// @tparam Function Callable type.
    /// @tparam Args Argument types forwarded to the function.
    /// @param func Callable to invoke on each event loop wakeup.
    /// @param args Arguments forwarded to `func`.
    /// @param loop Event loop to attach the async handle to.
    template <typename Function, typename... Args>
    explicit Synchronizer(Function&& func, Args&&... args, uv::Loop* loop = uv::defaultLoop())
        : _handle(loop)
    {
        start(std::forward<Function>(func),
              std::forward<Args>(args)...);
    }

    /// Destructor.
    virtual ~Synchronizer();

    /// Send a synchronization request to the event loop.
    /// Call this each time you want the target method called synchronously.
    /// The synchronous method will be called on next iteration.
    /// This is not atomic, so do not expect a callback for every request.
    void post();

    /// Starts the synchronizer with a variadic callback function.
    /// The callback is invoked from the event loop each time `post()` is called.
    /// Throws `std::logic_error` if already running or if the handle is null.
    /// @tparam Function Callable type.
    /// @tparam Args Argument types forwarded to the function.
    /// @param func Callable to invoke on each event loop wakeup.
    /// @param args Arguments forwarded to `func`.
    template <typename Function, typename... Args>
    void start(Function&& func, Args&&... args)
    {
        using Callback = internal::DeferredCallable<Function, Args...>;

        // assert(!_handle.active());
        if (_context->running)
            throw std::logic_error("Synchronizer is already running");

        _context->reset();
        _context->running = true;
        _context->repeating = true; // always repeating

        // Use a Callback instance since we can't pass the capture lambda
        // to the libuv callback without compiler trickery.
        if (!_handle.get())
            throw std::logic_error("Synchronizer handle is null");
        _handle.get()->data = new Callback(_context,
                                           std::forward<Function>(func),
                                           std::forward<Args>(args)...);
        _handle.init(&uv_async_init, [](uv_async_t* req) {
            auto wrap = reinterpret_cast<Callback*>(req->data);
            if (!wrap->ctx->cancelled) {
                wrap->invoke();
            } else {
                req->data = nullptr;
                wrap->ctx->running = false;
                delete wrap;
            }
        });
        _handle.throwLastError("Cannot initialize async");
        if (!_handle.active())
            throw std::logic_error("Synchronizer handle failed to become active");
    }

    /// Starts the synchronizer with a `std::function` callback.
    /// Overrides `Runner::start`; delegates to the variadic `start` template.
    /// @param func Callback invoked from the event loop on each `post()` call.
    void start(std::function<void()> func) override;

    /// Cancels the synchronizer, signalling the associated callback to stop.
    /// A subsequent `post()` is needed to wake up the event loop so it can process the cancellation.
    virtual void cancel();

    /// Cancels the synchronizer and closes the underlying `uv_async_t` handle.
    /// Safe to call multiple times; no-op if already closed.
    virtual void close();

    /// Returns a reference to the underlying libuv async handle.
    /// @return Reference to the `uv::Handle<uv_async_t>`.
    uv::Handle<uv_async_t>& handle();

protected:
    virtual bool async() const override;

    uv::Handle<uv_async_t> _handle;
};


} // namespace icy


/// @\}
