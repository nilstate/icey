///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/interface.h"
#include "icy/logger.h"
#include <chrono>
#include <memory>
#include <thread>


namespace icy {


Runner::Runner()
    : _context(std::make_shared<Runner::Context>())
{
}


Runner::~Runner()
{
    // Always cancel so the async context can exit ASAP.
    cancel();
}


void Runner::setRepeating(bool flag)
{
    if (_context->running)
        throw std::logic_error("Runner already running");
    _context->repeating = flag;
}


bool Runner::running() const
{
    return _context->running;
}


bool Runner::repeating() const
{
    return _context->repeating;
}


void Runner::cancel()
{
    _context->cancelled = true;
}


bool Runner::cancelled() const
{
    return _context->cancelled;
}


std::thread::id Runner::tid() const
{
    return _context->tid;
}


bool Runner::waitForExit(int timeout)
{
    if (Thread::currentID() == tid())
        throw std::logic_error("Runner: cannot join from own thread");
    using namespace std::chrono;
    auto start = steady_clock::now();
    while (!cancelled() || running()) {
        std::this_thread::sleep_for(milliseconds(10));
        if (timeout > 0) {
            auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
            if (elapsed > timeout) {
                throw std::logic_error("Runner: join timed out, possible deadlock");
                return false;
            }
        }
    }
    return true;
}


} // namespace icy


/// @}
