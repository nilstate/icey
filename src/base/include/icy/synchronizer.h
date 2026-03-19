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

    /// Create the synchronization context the given event loop.
    Synchronizer(uv::Loop* loop);

    /// Create the synchronization context the given event loop and method.
    /// The target method will be called from the event loop context.
    Synchronizer(std::function<void()> target, uv::Loop* loop = uv::defaultLoop());

    /// Create the synchronization context the given event loop and method.
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

    /// Start the synchronizer with the given callback.
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

    /// Start the synchronizer with the given callback function.
    void start(std::function<void()> func) override;

    virtual void cancel();

    virtual void close();
    // virtual bool closed();

    uv::Handle<uv_async_t>& handle();

protected:
    virtual bool async() const override;

    uv::Handle<uv_async_t> _handle;
};


} // namespace icy


/// @\}
