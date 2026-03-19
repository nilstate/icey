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
    Timeout(long delay = 0, bool autoStart = false);
    Timeout(const Timeout& src);
    Timeout(Timeout&& src) noexcept = default;
    ~Timeout();

    bool running() const;
    void start();
    void stop();
    void reset();
    long remaining() const;
    bool expired() const;

    void setDelay(long delay) { _delay = delay; };

    long delay() const { return _delay; };

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
    explicit TimedToken(long duration = 1000);
    explicit TimedToken(const std::string& id, long duration = 1000);

    std::string id() const { return _id; }

    bool operator==(const TimedToken& r) const { return _id == r._id; }
    bool operator==(std::string_view r) const { return _id == r; }

protected:
    std::string _id;
};


} // namespace icy
