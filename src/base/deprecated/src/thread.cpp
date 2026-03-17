///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/thread.h"
#include "assert.h"
#include "scy/logger.h"
#include "scy/platform.h"
#include <memory>


using std::endl;


namespace scy {


const uv_thread_t Thread::mainID = uv_thread_self();


Thread::Thread()
{
}


Thread::Thread(basic::Runnable& target)
{
    start(target);
}


Thread::Thread(std::function<void()> target)
{
    start(target);
}


Thread::Thread(std::function<void(void*)> target, void* arg)
{
    start(target, arg);
}


Thread::~Thread()
{
}


void Thread::startAsync()
{
    int r = uv_thread_create(
        &_handle,
        [](void* arg) {
            auto& ptr = *reinterpret_cast<Runner::Context::ptr*>(arg);
            ptr->tid = 0;
            ptr->exit = false;
            do {
                runAsync(ptr.get());
                scy::sleep(1); // TODO: uv_thread_yield when available
            } while (ptr->repeating && !ptr->cancelled());
            ptr->running = false;
            ptr->started = false;
            delete &ptr;
        },
        new Runner::Context::ptr(pContext));
    if (r < 0)
        throw std::runtime_error("System error: Cannot initialize thread");
}


void Thread::join()
{
    // WARNING: Do not use Logger in this method
    assert(this->tid() != Thread::currentID());
    // assert(this->cancelled()); // should probably be cancelled, but depends
    // on impl
    uv_thread_join(&_handle);
    assert(!this->running());
    assert(!this->started());
}


bool Thread::waitForExit(int timeout)
{
    // WARNING: Do not use Logger in this method
    assert(Thread::currentID() != this->tid());
    int times = 0;
    int interval = 10;
    while (!this->cancelled() || this->running()) {
        scy::sleep(interval);
        times++;
        if (timeout && ((times * interval) > timeout)) {
            assert(0 && "deadlock; calling inside thread scope?");
            return false;
        }
    }
    return true;
}


uv_thread_t Thread::currentID()
{
    return uv_thread_self();
}


bool Thread::async() const
{
    return true;
}


} // namespace scy


/// @\}
