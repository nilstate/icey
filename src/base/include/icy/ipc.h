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

    Callback target;  ///< The callable to invoke when the action is dispatched.
    void* arg;        ///< Optional opaque pointer passed to the callback.
    std::string data; ///< Optional string payload passed to the callback.

    /// Constructs an Action with the given callback, optional argument, and optional data.
    /// @param target Callback to invoke on dispatch.
    /// @param arg    Opaque pointer passed to the callback (default: nullptr).
    /// @param data   String payload passed to the callback (default: empty).
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

    /// Pushes an action onto the queue and triggers a post notification.
    /// Takes ownership of action; the queue deletes it after execution.
    /// Thread-safe.
    /// @param action Heap-allocated action to enqueue.
    virtual void push(TAction* action)
    {
        {
            std::lock_guard<std::mutex> guard(_mutex);
            _actions.push_back(action);
        }
        post();
    }

    /// Removes and returns the next action from the front of the queue.
    /// The caller takes ownership of the returned pointer.
    /// Thread-safe.
    /// @return Pointer to the next action, or nullptr if the queue is empty.
    virtual TAction* pop()
    {
        if (_actions.empty())
            return nullptr;
        std::lock_guard<std::mutex> guard(_mutex);
        TAction* next = _actions.front();
        _actions.pop_front();
        return next;
    }

    /// Drains the queue by invoking and deleting every pending action in order.
    /// Must be called from the thread that owns the event loop.
    virtual void runSync()
    {
        TAction* next = nullptr;
        while ((next = pop())) {
            next->target(*next);
            delete next;
        }
    }

    /// Closes the underlying notification handle. No-op in the base implementation.
    virtual void close() {}

    /// Signals the event loop that new actions are available. No-op in the base implementation.
    virtual void post() {}

    /// Blocks the calling thread until the queue is empty or the timeout elapses.
    /// Polls every 10 ms. Logs a warning if the timeout is reached.
    /// @param timeout Maximum time to wait (default: 5000 ms).
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
    /// Constructs a SyncQueue bound to the given libuv event loop.
    /// @param loop Event loop to synchronize with (default: the process-wide default loop).
    SyncQueue(uv::Loop* loop = uv::defaultLoop())
        : _sync([this]() { this->runSync(); }, loop)
    {
    }

    virtual ~SyncQueue() {}

    /// Closes the underlying Synchronizer handle and stops loop wakeups.
    virtual void close() { _sync.close(); }

    /// Wakes up the event loop so pending actions are dispatched via runSync().
    virtual void post() { _sync.post(); }

    /// Returns a reference to the internal Synchronizer.
    /// @return Reference to the Synchronizer used for loop wakeup.
    virtual Synchronizer& sync() { return _sync; }

protected:
    Synchronizer _sync;
};


using ActionQueue = ipc::Queue<ipc::Action>;
using ActionSyncQueue = ipc::SyncQueue<ipc::Action>;


} // namespace ipc
} // namespace icy


/// @\}
