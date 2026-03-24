///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/thread.h"
#include "icy/logger.h"
#include <memory>




namespace icy {


const std::thread::id Thread::mainID = std::this_thread::get_id();


Thread::Thread()
    : // Call the default constructor explicitly to
    // underline the fact that it does get called
    _thread()
{
}


Thread::~Thread()
{
    cancel();
    if (_thread.joinable())
        _thread.join();
}


void Thread::start(std::function<void()> target)
{
    start<std::function<void()>>(std::forward<std::function<void()>>(target));
}


void Thread::join()
{
    if (this->tid() == Thread::currentID())
        throw std::logic_error("Thread: cannot join from own thread");
    // assert(this->cancelled()); // should probably be cancelled
    _thread.join();
    if (this->running())
        LWarn("Thread still running after join");
}


std::thread::id Thread::currentID()
{
    return std::this_thread::get_id(); //uv_thread_self();
}


bool Thread::async() const
{
    return true;
}


} // namespace icy


/// @}
