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
#include "scy/signal.h"
#include <cstdint>
#include <functional>


namespace scy {


/// Asynchronous event based timer.
class Base_API Timer : public Runner
{
public:
    /// Create a timer.
    Timer(uv::Loop* loop = uv::defaultLoop());

    /// Create a timeout timer.
    ///
    /// The timeout timer will trigger once after `timeout` milliseconds.
    Timer(std::int64_t timeout, uv::Loop* loop = uv::defaultLoop(), std::function<void()> func = nullptr);

    /// Create a repeating interval timer.
    ///
    /// The interval timer will trigger once after `timeout` milliseconds,
    /// and continue to trigger after the `interval` milliseconds.
    Timer(std::int64_t timeout, std::int64_t interval, uv::Loop* loop = uv::defaultLoop(), std::function<void()> func = nullptr);

    /// Destructor.
    virtual ~Timer();

    /// Start the timer.
    void start();

    /// Start the timer with the given callback function.
    void start(std::function<void()> func);

    /// Stop the timer.
    void stop();

    /// Restart the timer.
    ///
    /// This method works even if it hasn't been started yet.
    /// An interval or interval must be set or an exception will be thrown.
    void restart();

    /// Trigger the timer again.
    ///
    /// If the timer is repeating restart it using the
    /// repeat value as the `timeout`. If the timer has never been started
    /// before it returns -1 and sets the error to `UV_EINVAL`.
    void again();

    /// Set the timeout value.
    ///
    /// The timer must not be active when this value is set.
    void setTimeout(std::int64_t timeout);

    /// Set the repeat value.
    ///
    /// Note that if the repeat value is set from a timer callback it does not
    /// immediately take effect. If the timer was non-repeating before, it will
    /// have been stopped. If it was repeating, then the old repeat value will
    /// have been used to schedule the next timeout.
    void setInterval(std::int64_t interval);

    bool active() const;

    std::int64_t timeout() const;
    std::int64_t interval() const;
    std::int64_t count();

    uv::Handle<uv_timer_t>& handle();

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


} // namespace scy


/// @\}
