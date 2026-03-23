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
#include "icy/datetime.h"
#include "icy/interface.h"
#include "icy/platform.h"
#include "icy/synchronizer.h"
#include "icy/thread.h"
#include <queue>
#include <stdexcept>


namespace icy {


/// Thread-safe queue container.
template <typename T>
class Queue
{
public:
    /// Appends an item to the back of the queue (thread-safe).
    /// @param data Item to enqueue.
    void push(const T& data)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _queue.push_back(data);
    }

    /// Appends an item to the back of the queue by move (thread-safe).
    /// @param data Item to enqueue.
    void push(T&& data)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _queue.push_back(std::move(data));
    }

    /// @return True if the queue contains no items.
    bool empty() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _queue.empty();
    }

    /// @return Copy of the front item.
    T front() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _queue.front();
    }

    /// @return Copy of the back item.
    T back() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _queue.back();
    }

    /// Removes the front item from the queue (thread-safe).
    void pop()
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _queue.pop_front();
    }

    /// Sorts all queued items using the given comparator (thread-safe).
    /// @tparam Compare Comparator type compatible with `std::sort`.
    template <typename Compare>
    void sort()
    {
        std::lock_guard<std::mutex> guard(_mutex);
        std::sort(_queue.begin(), _queue.end(), Compare());
    }

    /// @return Number of items currently in the queue (thread-safe).
    size_t size() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _queue.size();
    }

    /// @return Copy of the underlying deque.
    std::deque<T> queue() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _queue;
    }

protected:
    std::deque<T> _queue;
    mutable std::mutex _mutex;
};


//
// Runnable Queue
//


/// Queue of runnable tasks for sequential execution
template <class T>
class RunnableQueue : public Queue<T*>
    , public basic::Runnable
{
public:
    /// The default dispatch function.
    /// Must be set before the queue is running.
    std::function<void(T&)> ondispatch;

    /// @param limit   Maximum number of queued items; oldest are purged when exceeded (0 = unlimited).
    /// @param timeout Dispatch timeout in milliseconds; 0 means run until cancel() is called.
    RunnableQueue(int limit = 2048, int timeout = 0)
        : _limit(limit)
        , _timeout(timeout)
    {
    }

    virtual ~RunnableQueue()
    {
        clear();
    }

    /// Push an item onto the queue.
    /// The queue takes ownership of the item pointer.
    virtual void push(T* item)
    {
        std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);

        while (_limit > 0 && static_cast<int>(Queue<T*>::_queue.size()) >= _limit) {
            LWarn("Purging: ", Queue<T*>::_queue.size());
            delete Queue<T*>::_queue.front();
            Queue<T*>::_queue.pop_front();
            ++_dropped;
        }

        Queue<T*>::_queue.push_back(item);
    }

    /// Flush all outgoing items.
    virtual void flush()
    {
        while (true) {
            T* next = nullptr;
            {
                std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);
                if (Queue<T*>::_queue.empty())
                    break;
                next = Queue<T*>::_queue.front();
                Queue<T*>::_queue.pop_front();
            }

            dispatch(*next);
            delete next;
        }
    }

    // Clear all queued items.
    void clear()
    {
        std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);
        while (!Queue<T*>::_queue.empty()) {
            delete Queue<T*>::_queue.front();
            Queue<T*>::_queue.pop_front();
        }
    }

    /// Called asynchronously to dispatch queued items.
    /// If not timeout is set this method blocks until cancel() is called,
    /// otherwise runTimeout() will be called.
    void run() override
    {
        if (_timeout) {
            runTimeout();
        } else {
            while (!cancelled()) {
                if (!dispatchNext())
                    std::this_thread::yield();
            }
        }
    }

    /// Called asynchronously to dispatch queued items
    /// until the queue is empty or the timeout expires.
    /// Pseudo protected for std::bind compatability.
    virtual void runTimeout()
    {
        Stopwatch sw;
        sw.start();
        do {
            // icy::sleep(1);
        } while (!cancelled() && sw.elapsedMilliseconds() < _timeout && dispatchNext());
    }

    /// Dispatch a single item to listeners.
    virtual void dispatch(T& item)
    {
        if (ondispatch)
            ondispatch(item);
    }

    /// @return Current dispatch timeout in milliseconds.
    int timeout()
    {
        std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);
        return _timeout;
    }

    /// Sets the dispatch timeout. Must only be called when the queue is empty.
    /// @param milliseconds New timeout in milliseconds.
    /// @throws std::logic_error if the queue is non-empty.
    void setTimeout(int milliseconds)
    {
        std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);
        if (!Queue<T*>::_queue.empty())
            throw std::logic_error("Cannot change timeout while queue is active");
        _timeout = milliseconds;
    }

    /// @return Number of items purged because the queue limit was exceeded.
    size_t dropped() const
    {
        std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);
        return _dropped;
    }

protected:
    RunnableQueue(const RunnableQueue&) = delete;
    RunnableQueue& operator=(const RunnableQueue&) = delete;
    RunnableQueue(RunnableQueue&&) = delete;
    RunnableQueue& operator=(RunnableQueue&&) = delete;

    /// Pops the next waiting item.
    virtual T* popNext()
    {
        T* next = nullptr;
        {
            std::lock_guard<std::mutex> guard(Queue<T*>::_mutex);
            if (Queue<T*>::_queue.empty())
                return nullptr;

            next = Queue<T*>::_queue.front();
            Queue<T*>::_queue.pop_front();
        }
        return next;
    }

    /// Pops and dispatches the next waiting item.
    virtual bool dispatchNext()
    {
        T* next = popNext();
        if (next) {
            dispatch(*next);
            delete next;
            return true;
        }
        return false;
    }

    int _limit;
    int _timeout;
    size_t _dropped = 0;
};


//
// Synchronization Queue
//


/// SyncQueue extends Synchronizer to implement a synchronized FIFO
/// queue which receives T objects from any thread and synchronizes
/// them for safe consumption by the associated event loop.
template <class T>
class SyncQueue : public RunnableQueue<T>
{
public:
    using Queue = RunnableQueue<T>;

    /// @param loop    Event loop used by the internal `Synchronizer`.
    /// @param limit   Maximum queued items; oldest are dropped when exceeded.
    /// @param timeout Dispatch timeout in milliseconds passed to `RunnableQueue`.
    SyncQueue(uv::Loop* loop, int limit = 2048, int timeout = 20)
        : Queue(limit, timeout)
        , _sync([this]() { this->run(); }, loop)
    {
    }

    /// Destruction is deferred to allow enough
    /// time for all callbacks to return.
    virtual ~SyncQueue()
    {
    }

    /// Pushes an item onto the queue and wakes the event loop for dispatch.
    /// Ownership of `item` is transferred to the queue.
    /// @param item Heap-allocated item to enqueue; the queue takes ownership.
    void push(T* item) override
    {
        Queue::push(item);
        _sync.post();
    }

    /// Cancels the queue and its underlying synchronizer.
    /// @param flag True to cancel, false to un-cancel.
    void cancel(bool flag = true) override
    {
        Queue::cancel(flag);
        _sync.cancel(); // Call uv_close on the handle if calling from
                        // the event loop thread or we deadlock.
        if (_sync.tid() == Thread::currentID())
            _sync.close();
    }

    /// @return Reference to the underlying `Synchronizer` handle.
    Synchronizer& sync() { return _sync; }

protected:
    Synchronizer _sync;
};


//
// Asynchronous Queue
//


/// AsyncQueue is a thread-based queue which receives packets
/// from any thread source and dispatches them asynchronously.
///
/// This queue is useful for deferring load from operation
/// critical system devices before performing long running tasks.
///
/// The thread will call the RunnableQueue's run() method to
/// constantly flush outgoing packets until cancel() is called.
template <class T>
class AsyncQueue : public RunnableQueue<T>
{
public:
    using Queue = RunnableQueue<T>;

    /// @param limit Maximum number of queued items before oldest are dropped.
    AsyncQueue(int limit = 2048)
        : Queue(limit)
        , _thread([this]() { this->run(); })
    {
    }

    /// Cancels the queue and joins the dispatch thread.
    /// @param flag True to cancel, false to un-cancel.
    void cancel(bool flag = true) override
    {
        Queue::cancel(flag);
        _thread.cancel();
    }

protected:
    virtual ~AsyncQueue()
    {
    }

    Thread _thread;
};


} // namespace icy


/// @\}
