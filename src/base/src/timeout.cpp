///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{
//


#include "scy/timeout.h"
#include "scy/util.h"


namespace scy {


//
// Timeout
//


Timeout::Timeout(long delay, bool autoStart)
    : _startAt()
    , _delay(delay)
    , _running(false)
{
    if (autoStart)
        start();
}


Timeout::Timeout(const Timeout& src)
    : _startAt(src._startAt)
    , _delay(src._delay)
    , _running(src._running)
{
}


Timeout& Timeout::operator=(const Timeout& src)
{
    _startAt = src._startAt;
    _delay = src._delay;
    _running = src._running;
    return *this;
}


Timeout::~Timeout()
{
}


bool Timeout::running() const
{
    return _running;
}


void Timeout::start()
{
    _startAt = std::chrono::steady_clock::now();
    _running = true;
}


void Timeout::stop()
{
    _running = false;
}


void Timeout::reset()
{
    _startAt = std::chrono::steady_clock::now();
    _running = true;
}


long Timeout::remaining() const
{
    if (!_running)
        return _delay;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startAt).count();
    long rem = _delay - static_cast<long>(elapsed);
    return rem > 0 ? rem : 0;
}


bool Timeout::expired() const
{
    if (!_running)
        return false;

    return remaining() == 0;
}


//
// Timed Token
//


TimedToken::TimedToken(long duration)
    : Timeout(duration, true)
    , _id(util::randomString(32))
{
}


TimedToken::TimedToken(const std::string& id, long duration)
    : Timeout(duration, true)
    , _id(id)
{
}


} // namespace scy


/// @\}
