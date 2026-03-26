///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/handle.h"
#include "icy/runner.h"

#include <functional>
#include <stdexcept>


namespace icy {


/// Asynchronous type that triggers callbacks when the event loop is idle.
///
/// This class inherits the `Runner` interface and may be used with any
/// implementation that's powered by an asynchronous `Runner`.
///
class Base_API Idler : public Runner
{
public:
    /// Create the idler bound to @p loop without starting it.
    ///
    /// @param loop  Event loop to associate with. Defaults to the process-wide default loop.
    Idler(uv::Loop* loop = uv::defaultLoop());

    /// Create and immediately start the idler on the default loop.
    ///
    /// @param func  Callable invoked on every idle iteration.
    /// @param args  Arguments forwarded to @p func.
    template <typename Function, typename... Args>
    explicit Idler(Function&& func, Args&&... args)
        : _handle(uv::defaultLoop())
    {
        init();
        start(std::forward<Function>(func), std::forward<Args>(args)...);
    }

    /// Create and immediately start the idler on the given loop.
    ///
    /// @param loop  Event loop to associate with.
    /// @param func  Callable invoked on every idle iteration.
    /// @param args  Arguments forwarded to @p func.
    template <typename Function, typename... Args>
    explicit Idler(uv::Loop* loop, Function&& func, Args&&... args)
        : _handle(loop)
    {
        init();
        start(std::forward<Function>(func), std::forward<Args>(args)...);
    }

    /// Start the idler, invoking @p func (with @p args) on every idle iteration.
    ///
    /// The idler always runs in repeating mode; cancel via `Runner::cancel()`.
    /// Throws `std::logic_error` if the handle is already active or the runner
    /// context reports it is still running.
    ///
    /// @param func  Callable invoked each time the event loop is idle.
    /// @param args  Arguments forwarded to @p func on each invocation.
    template <typename Function, typename... Args>
    void start(Function&& func, Args&&... args)
    {
        using Callback = internal::DeferredCallable<Function, Args...>;

        if (_handle.active())
            throw std::logic_error("Idler handle is already active");
        if (_context->running)
            throw std::logic_error("Idler is already running");

        _context->reset();
        _context->running = true;
        _context->repeating = true; // idler is always repeating

        // Use a Callback instance since we can't pass the capture lambda
        // to the libuv callback without compiler trickery.
        auto* cb = new Callback(_context,
                                std::forward<Function>(func),
                                std::forward<Args>(args)...);
        _handle.get()->data = cb;
        _handle.setCloseCleanup(cb);
        _handle.invoke(&uv_idle_start, _handle.get(),
                       [](uv_idle_t* req) {
                           auto* wrap = reinterpret_cast<Callback*>(req->data);
                           if (!wrap)
                               return;

                           if (!wrap->ctx->cancelled)
                               wrap->invoke();

                           if (wrap->ctx->cancelled) {
                               wrap->ctx->running = false;
                               uv_idle_stop(req);
                               if (!uv_is_closing(reinterpret_cast<uv_handle_t*>(req))) {
                                   req->data = nullptr;
                                   uv::clearHandleCloseCleanup(req);
                                   delete wrap;
                               }
                           }
                       });
        _handle.throwLastError("Cannot start idler");
    }

    /// Start the idler with a type-erased callback (implements `Runner::start`).
    ///
    /// @param func  Callback invoked on every idle iteration.
    void start(std::function<void()> func) override;

    virtual ~Idler() = default;

    /// @return  Reference to the underlying `uv_idle_t` handle wrapper.
    uv::Handle<uv_idle_t>& handle();

protected:
    /// Initialize the underlying `uv_idle_t` handle and unref it from the loop.
    virtual void init();

    /// @return  `false`; the idler is event-loop-driven, not thread-based.
    bool async() const override;

    uv::Handle<uv_idle_t> _handle;
};


} // namespace icy


/// @}
