///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/timer.h"
#include "scy/logger.h"
#include "scy/platform.h"


using std::endl;


namespace scy {


Timer::Timer(uv::Loop* loop)
    : _handle(loop) //, new uv_timer_t)
    , _timeout(0)
    , _interval(0)
    , _count(0)
{
    // LTrace("Create");
    init();
}


Timer::Timer(std::int64_t timeout, uv::Loop* loop, std::function<void()> func)
    : _handle(loop)
    , _timeout(timeout)
    , _interval(0)
    , _count(0)
{
    // LTrace("Create");
    init();
    if (func)
        start(func);
}


Timer::Timer(std::int64_t timeout, std::int64_t interval, uv::Loop* loop, std::function<void()> func)
    : _handle(loop)
    , _timeout(timeout)
    , _interval(interval)
    , _count(0)
{
    // LTrace("Create");
    init();
    if (func)
        start(func);
}


Timer::~Timer()
{
    // LTrace("Destroy");
}


void Timer::init()
{
    // LTrace("Init");

    // _count = 0;
    // _timeout = 0;
    // _interval = 0;

    if (!_handle.get())
        throw std::logic_error("Timer handle not allocated");
    _handle.get()->data = this;
    _handle.init(&uv_timer_init);
    _handle.throwLastError("Cannot initialize timer");
    _handle.unref(); // unref by default
}


// void Timer::start(std::int64_t interval)
// {
//     start(interval, interval);
// }


void Timer::start(std::function<void()> func)
{
    start();
    Timeout += func;
}


void Timer::start()
{
    // LTrace("Starting: ",,  timeout, ": ", interval);
    if (active()) return;
    if (!_handle.get())
        throw std::logic_error("Timer handle not allocated");
    if (_timeout <= 0 && _interval <= 0)
        throw std::logic_error("Timer: no timeout or interval set");

    // _timeout = timeout;
    // _interval = interval;
    _count = 0;

    _handle.invoke(&uv_timer_start, _handle.get(), [](uv_timer_t* req) {
            auto self = reinterpret_cast<Timer*>(req->data);
            self->_count++;
            self->Timeout.emit(); }, _timeout, _interval);
    _handle.throwLastError("Cannot start timer");
}


void Timer::stop()
{
    // LTrace("Stopping");

    if (!active())
        return; // do nothing

    _count = 0;
    _handle.invoke(&uv_timer_stop, _handle.get());
    _handle.throwLastError("Cannot stop timer");
}


void Timer::restart()
{
    // STrace, "Restarting: ", __handle.ptr)
    if (!active())
        return start(); //_timeout, _interval);
    return again();
}


void Timer::again()
{
    // LTrace("Again");

    if (!_handle.get())
        throw std::logic_error("Timer handle not allocated");
    _handle.invoke(&uv_timer_again, _handle.get());
    _handle.throwLastError("Cannot run timer again");

    //assert(active());
    _count = 0;
}


void Timer::setTimeout(std::int64_t timeout)
{
    if (active()) return;
    _timeout = timeout;
}


void Timer::setInterval(std::int64_t interval)
{
    if (!_handle.get())
        throw std::logic_error("Timer handle not allocated");
    _interval = interval;
    uv_timer_set_repeat(_handle.get(), interval);
}


bool Timer::active() const
{
    return _handle.active();
}


std::int64_t Timer::timeout() const
{
    return _timeout;
}


std::int64_t Timer::interval() const
{
    if (!_handle.get())
        throw std::logic_error("Timer handle not allocated");
    return std::min<std::int64_t>(
        uv_timer_get_repeat(_handle.get()), 0);
}


std::int64_t Timer::count()
{
    return _count;
}


bool Timer::async() const
{
    return false;
}


uv::Handle<uv_timer_t>& Timer::handle()
{
    return _handle;
}


} // namespace scy


/// @\}
