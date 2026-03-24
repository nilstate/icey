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
#include "icy/interface.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <tuple>


namespace icy {


/// Runner is a virtual interface for implementing
/// asynchronous objects such as threads and futures.
class Base_API Runner
{
public:
    Runner();
    virtual ~Runner();

    /// Starts the asynchronous context with the given callback.
    /// The callback must remain valid for the lifetime of the `Runner`.
    /// @param target Callable to invoke when the context runs.
    virtual void start(std::function<void()> target) = 0;

    /// Returns true if the async context is currently running.
    /// @return True if the runner's context has been started and has not yet stopped.
    bool running() const;

    /// Signals the async context to stop at the earliest opportunity.
    void cancel();

    /// Returns true if the context has been cancelled.
    /// The implementation is responsible for exiting as soon as possible after cancellation.
    /// @return True if `cancel()` has been called.
    bool cancelled() const;

    /// Returns true if the runner is in repeating mode.
    /// @return True if the target function is invoked in a loop until cancelled.
    bool repeating() const;

    /// Enables or disables repeating mode.
    /// When repeating, the target function is invoked repeatedly until `cancel()` is called.
    /// This normalizes behaviour across thread-based and event-loop-based `Runner` implementations.
    /// Must be called before `start()`.
    /// @param flag True to enable repeating mode, false to run the target once.
    void setRepeating(bool flag);

    /// Returns true if the implementation is thread-based.
    /// @return True for thread-backed runners, false for event-loop-driven runners.
    virtual bool async() const = 0;

    /// Returns the native thread ID of the thread running the async context.
    /// @return `std::thread::id` of the runner thread, or a default-constructed ID if not started.
    std::thread::id tid() const;

    /// Blocks until the async context exits or the timeout elapses.
    /// The context should be cancelled before calling this method.
    /// Must be called from outside the runner's own thread to avoid deadlock.
    /// @param timeout Maximum number of milliseconds to wait. Pass 0 to wait indefinitely.
    /// @return True if the context exited cleanly, false if the timeout was reached (throws instead).
    bool waitForExit(int timeout = 5000);

    /// Context object which we send to the thread context.
    ///
    /// This intermediate object allows us to garecefully handle late invokebacks
    /// and so avoid the need for deferred destruction of `Runner` objects.
    struct Context
    {
        std::thread::id tid;
        std::atomic<bool> running;
        std::atomic<bool> cancelled;
        bool repeating = false;

        /// Resets the context to its initial state so it can be reused.
        /// The implementation must call this before restarting a `Runner`.
        void reset()
        {
            running = false;
            cancelled = false;
        }

        /// Default constructor; calls `reset()` to initialize fields.
        Context()
        {
            reset();
        }
    };

protected:
    /// NonCopyable and NonMovable
    Runner(const Runner&) = delete;
    Runner& operator=(const Runner&) = delete;
    Runner(Runner&&) = delete;
    Runner& operator=(Runner&&) = delete;

    /// Shared pointer to the internal Context.
    std::shared_ptr<Context> _context;
};


//
// Internal Helpers
//


namespace internal {


/// Runs a function inside a `Runner::Context`, setting running/tid state and
/// looping if `repeating` is set. Catches exceptions if `ICY_EXCEPTION_RECOVERY` is defined.
/// @param c Shared context used to track running/cancelled/repeating state.
/// @param func Callable to invoke.
/// @param args Arguments forwarded to `func`.
template <typename Function, typename... Args>
inline void runAsync(std::shared_ptr<Runner::Context> c, Function func, Args... args)
{
    c->tid = std::this_thread::get_id();
    c->running = true;
    do {
#ifdef ICY_EXCEPTION_RECOVERY
        try {
#endif
            func(std::forward<Args>(args)...);
#ifdef ICY_EXCEPTION_RECOVERY
        } catch (std::exception& exc) {
            std::cerr << "Runner exception: " << exc.what() << '\n';
        }
#endif
        if (c->repeating && !c->cancelled)
            std::this_thread::yield();
    } while (c->repeating && !c->cancelled);
    c->running = false;
}


/// Helper class that stores a function pointer and variadic arguments as a tuple
/// for deferred invocation.
template <typename Function, typename... Args>
struct DeferredCallable
{
    std::shared_ptr<Runner::Context> ctx;
    Function func;
    std::tuple<Args...> args;

    /// @param c Shared runner context for cancellation checks.
    /// @param f Callable to store.
    /// @param a Arguments to store alongside the callable.
    DeferredCallable(std::shared_ptr<Runner::Context> c, Function&& f, Args&&... a)
        : ctx(c)
        , func(f)
        , args(std::make_tuple(a...))
    {
    }

    /// Invokes the stored callable with the stored arguments.
    void invoke()
    {
        std::apply(func, args);
    }
};


} // namespace internal
} // namespace icy


/// @}
