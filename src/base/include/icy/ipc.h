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
#include "icy/synchronizer.h"

#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <thread>


namespace icy {


/// Classes for inter-process communication.
namespace ipc {


/// Default action type for executing synchronized callbacks.
struct Action
{
    using Callback = std::function<void(const Action&)>;

    Callback target;
    void* arg;
    std::string data;

    Action(Callback target, void* arg = nullptr, const std::string& data = "")
        : target(target)
        , arg(arg)
        , data(data)
    {
    }
};


/// IPC queue is for safely passing templated
/// actions between threads and processes.
template <typename TAction = ipc::Action>
class Queue
{
public:
    Queue() {}

    virtual ~Queue() {}

    virtual void push(TAction* action)
    {
        {
            std::lock_guard<std::mutex> guard(_mutex);
            _actions.push_back(action);
        }
        post();
    }

    virtual TAction* pop()
    {
        if (_actions.empty())
            return nullptr;
        std::lock_guard<std::mutex> guard(_mutex);
        TAction* next = _actions.front();
        _actions.pop_front();
        return next;
    }

    virtual void runSync()
    {
        TAction* next = nullptr;
        while ((next = pop())) {
            next->target(*next);
            delete next;
        }
    }

    virtual void close() {}

    virtual void post() {}

    void waitForSync(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000))
    {
        auto deadline = std::chrono::steady_clock::now() + timeout;
        while (std::chrono::steady_clock::now() < deadline) {
            {
                std::lock_guard<std::mutex> guard(_mutex);
                if (_actions.empty())
                    return;
            }
            LDebug("Wait for sync");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        LWarn("waitForSync timed out after ", timeout.count(), "ms");
    }

protected:
    mutable std::mutex _mutex;
    std::deque<TAction*> _actions;
};


/// IPC synchronization queue is for passing templated
/// actions between threads and the event loop we are
/// synchronizing with.
///
template <typename TAction = ipc::Action>
class SyncQueue : public Queue<TAction>
{
public:
    SyncQueue(uv::Loop* loop = uv::defaultLoop())
        : _sync([this]() { this->runSync(); }, loop)
    {
    }

    virtual ~SyncQueue() {}

    virtual void close() { _sync.close(); }

    virtual void post() { _sync.post(); }

    virtual Synchronizer& sync() { return _sync; }

protected:
    Synchronizer _sync;
};


using ActionQueue = ipc::Queue<ipc::Action>;
using ActionSyncQueue = ipc::SyncQueue<ipc::Action>;


} // namespace ipc
} // namespace icy


/// @\}
