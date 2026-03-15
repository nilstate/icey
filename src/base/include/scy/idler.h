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
#include "scy/handle.h"
#include "scy/runner.h"

#include <cassert>
#include <functional>


namespace scy {


/// Asynchronous type that triggers callbacks when the event loop is idle.
///
/// This class inherits the `Runner` interface and may be used with any
/// implementation that's powered by an asynchronous `Runner`.
///
class Base_API Idler : public Runner
{
public:
    /// Create the idler with the given event loop.
    Idler(uv::Loop* loop = uv::defaultLoop());

    /// Create and start the idler with the given callback.
    template <typename Function, typename... Args>
    explicit Idler(Function&& func, Args&&... args)
        : _handle(uv::defaultLoop())
    {
        init();
        start(std::forward<Function>(func), std::forward<Args>(args)...);
    }

    /// Create and start the idler with the given callback and event loop.
    template <typename Function, typename... Args>
    explicit Idler(uv::Loop* loop, Function&& func, Args&&... args)
        : _handle(loop)
    {
        init();
        start(std::forward<Function>(func), std::forward<Args>(args)...);
    }

    /// Start the idler with the given callback function.
    template <typename Function, typename... Args>
    void start(Function&& func, Args&&... args)
    {
        using Callback = internal::DeferredCallable<Function, Args...>;

        assert(!_handle.active());
        assert(!_handle.active());
        assert(!_context->running);

        _context->reset();
        _context->running = true;
        _context->repeating = true; // idler is always repeating

        // Use a Callback instance since we can't pass the capture lambda
        // to the libuv callback without compiler trickery.
        _handle.get()->data = new Callback(_context,
                                           std::forward<Function>(func),
                                           std::forward<Args>(args)...);
        _handle.invoke(&uv_idle_start, _handle.get(),
                       [](uv_idle_t* req) {
                           auto wrap = reinterpret_cast<Callback*>(req->data);
                           if (!wrap->ctx->cancelled) {
                               wrap->invoke();
                           } else {
                               wrap->ctx->running = false;
                               uv_idle_stop(req);
                               delete wrap;
                           }
                       });
        _handle.throwLastError("Cannot start idler");
    }

    /// Start the idler with the given callback function.
    virtual void start(std::function<void()> func);

    virtual ~Idler() = default;

    uv::Handle<uv_idle_t>& handle();

protected:
    virtual void init();

    virtual bool async() const override;

    uv::Handle<uv_idle_t> _handle;
};


} // namespace scy


/// @\}
