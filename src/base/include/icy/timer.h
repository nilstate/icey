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
#include "icy/signal.h"
#include <cstdint>
#include <functional>


namespace icy {


/// Asynchronous event based timer.
class Base_API Timer : public Runner
{
public:
    /// Create a timer without a timeout or interval; values must be set before `start()`.
    ///
    /// @param loop  Event loop to associate with. Defaults to the process-wide default loop.
    Timer(uv::Loop* loop = uv::defaultLoop());

    /// Create a one-shot timer that fires after @p timeout milliseconds.
    ///
    /// If @p func is provided the timer starts immediately; otherwise call
    /// `start()` manually.
    ///
    /// @param timeout  Delay in milliseconds before the first (and only) fire.
    /// @param loop     Event loop to associate with.
    /// @param func     Optional callback connected to the `Timeout` signal and
    ///                 used to start the timer immediately.
    Timer(std::int64_t timeout, uv::Loop* loop = uv::defaultLoop(), std::function<void()> func = nullptr);

    /// Create a repeating timer with an initial delay and a repeat period.
    ///
    /// Fires once after @p timeout milliseconds, then repeatedly every
    /// @p interval milliseconds. If @p func is provided the timer starts
    /// immediately; otherwise call `start()` manually.
    ///
    /// @param timeout   Initial delay in milliseconds.
    /// @param interval  Repeat period in milliseconds.
    /// @param loop      Event loop to associate with.
    /// @param func      Optional callback connected to the `Timeout` signal and
    ///                  used to start the timer immediately.
    Timer(std::int64_t timeout, std::int64_t interval, uv::Loop* loop = uv::defaultLoop(), std::function<void()> func = nullptr);

    /// Destructor.
    virtual ~Timer();

    /// Start the timer using the previously configured timeout and interval.
    ///
    /// Throws `std::logic_error` if neither a timeout nor an interval has been
    /// set, or if the handle has not been allocated. Has no effect if already
    /// active.
    void start();

    /// Connect @p func to the `Timeout` signal and start the timer.
    ///
    /// @param func  Callback invoked on each timeout event.
    void start(std::function<void()> func) override;

    /// Stop the timer. Resets the fire count to zero.
    ///
    /// Has no effect if the timer is not active.
    void stop();

    /// Restart the timer.
    ///
    /// If the timer is not currently active, behaves like `start()`. If it is
    /// active, calls `again()` to reset the countdown using the repeat value.
    /// A timeout or interval must be set beforehand, otherwise `start()` will
    /// throw `std::logic_error`.
    void restart();

    /// Reset the countdown without stopping and restarting the timer.
    ///
    /// For repeating timers this restarts the repeat interval from now.
    /// For non-repeating timers this has the same effect as stopping the timer.
    /// Throws on error if `uv_timer_again` fails (e.g. the timer was never
    /// started). Resets the fire count to zero.
    void again();

    /// Set the initial delay before the first timeout event.
    ///
    /// Has no effect if the timer is currently active.
    ///
    /// @param timeout  Delay in milliseconds.
    void setTimeout(std::int64_t timeout);

    /// Set the repeat interval (calls `uv_timer_set_repeat`).
    ///
    /// Takes effect from the next timeout event. If set from within a timer
    /// callback on a repeating timer, the new value is used from the following
    /// iteration; on a non-repeating timer the timer will have already stopped.
    ///
    /// @param interval  Repeat period in milliseconds; 0 disables repeating.
    void setInterval(std::int64_t interval);

    /// @return  `true` if the timer is currently counting down.
    bool active() const;

    /// @return  The configured initial timeout in milliseconds.
    std::int64_t timeout() const;

    /// @return  The effective repeat interval reported by libuv, in milliseconds.
    std::int64_t interval() const;

    /// @return  The number of times the timer has fired since the last start or again().
    std::int64_t count();

    /// @return  Reference to the underlying `uv_timer_t` handle wrapper.
    uv::Handle<uv_timer_t>& handle();

    /// @return  `false`; the timer is event-loop-driven, not thread-based.
    bool async() const override;

    /// Signal that gets triggered on timeout.
    NullSignal Timeout;

protected:
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    void init();

    uv::Handle<uv_timer_t> _handle;
    std::int64_t _timeout;
    std::int64_t _interval;
    std::int64_t _count;
};


} // namespace icy


/// @}
