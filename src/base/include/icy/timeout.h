///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{
//


#pragma once


#include "icy/base.h"
#include <chrono>
#include <string>
#include <string_view>


namespace icy {


//
// Timeout
//


/// Timeout counter which expires after a given delay.
/// Delay is specified in milliseconds.
class Timeout
{
public:
    /// Constructs a Timeout with the given delay.
    /// @param delay     Expiry duration in milliseconds (default: 0).
    /// @param autoStart If true, starts the timer immediately on construction.
    Timeout(long delay = 0, bool autoStart = false);

    /// Copy constructor.
    /// @param src Source Timeout to copy state from.
    Timeout(const Timeout& src);

    Timeout(Timeout&& src) noexcept = default;
    ~Timeout();

    /// Returns true if the timer is currently running.
    /// @return true if start() has been called and stop() has not.
    bool running() const;

    /// Starts (or restarts) the timer, recording the current time as the start point.
    void start();

    /// Stops the timer without resetting it. expired() will return false after this call.
    void stop();

    /// Restarts the timer from now, equivalent to calling start().
    void reset();

    /// Returns the number of milliseconds remaining before expiry.
    /// Returns 0 if already expired, or the full delay if not running.
    /// @return Milliseconds until expiry.
    long remaining() const;

    /// Returns true if the timer is running and the delay has fully elapsed.
    /// @return true if expired, false if stopped or not yet elapsed.
    bool expired() const;

    /// Sets the expiry delay without restarting the timer.
    /// @param delay New delay in milliseconds.
    void setDelay(long delay) { _delay = delay; };

    /// Returns the configured delay in milliseconds.
    /// @return Delay in milliseconds.
    long delay() const { return _delay; };

    /// Copy assignment operator.
    /// @param src Source Timeout to copy state from.
    Timeout& operator=(const Timeout& src);
    Timeout& operator=(Timeout&& src) noexcept = default;

protected:
    std::chrono::steady_clock::time_point _startAt;
    long _delay;
    bool _running;
};


//
// Timed Token
//


/// Token that expires after the specified duration.
class Base_API TimedToken : public Timeout
{
public:
    /// Constructs a TimedToken with a randomly generated 32-character ID,
    /// started immediately with the given duration.
    /// @param duration Lifetime in milliseconds (default: 1000).
    explicit TimedToken(long duration = 1000);

    /// Constructs a TimedToken with an explicit ID, started immediately.
    /// @param id       Token identifier.
    /// @param duration Lifetime in milliseconds (default: 1000).
    explicit TimedToken(const std::string& id, long duration = 1000);

    /// Returns the token's identifier string.
    /// @return Token ID.
    std::string id() const { return _id; }

    /// Compares two tokens by ID.
    bool operator==(const TimedToken& r) const { return _id == r._id; }

    /// Compares this token's ID against a string.
    bool operator==(std::string_view r) const { return _id == r; }

protected:
    std::string _id;
};


} // namespace icy
