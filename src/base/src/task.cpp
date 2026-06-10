///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/task.h"
#include "icy/logger.h"
#include "icy/platform.h"
#include "icy/thread.h"
#include "icy/util.h"

#include <iostream>
#include <thread>




namespace icy {


//
// Task
//


Task::Task(bool repeat)
    : _id(util::randomNumber())
    , _repeating(repeat)
    , _destroyed(false)
{
}


Task::~Task()
{
}


void Task::destroy()
{
    _destroyed = true;
}


uint32_t Task::id() const
{
    return _id;
}


bool Task::destroyed() const
{
    return _destroyed;
}


bool Task::repeating() const
{
    return _repeating;
}


//
// Task Runner
//


TaskRunner::TaskRunner(std::shared_ptr<Runner> runner)
{
    if (runner)
        setRunner(runner);
    else
        setRunner(std::make_shared<Thread>());
}


TaskRunner::~TaskRunner()
{
    Shutdown.emit();
    if (_runner) {
        _runner->cancel();
        // Wait for the worker to leave run() before deleting tasks it may
        // still be executing.
        try {
            if (_runner->async() && Thread::currentID() != _runner->tid())
                _runner->waitForExit();
        } catch (...) {
            // Timed out; proceed with teardown rather than throwing.
        }
    }
    clear();
}


bool TaskRunner::start(Task* task)
{
    add(task);
    LTrace("Start task: ", task);
    onStart(task);
    return true;
}


bool TaskRunner::cancel(Task* task)
{
    if (!task->cancelled()) {
        task->cancel();
        LTrace("Cancel task: ", task);
        onCancel(task);
        return true;
    }
    return false;
}


bool TaskRunner::destroy(Task* task)
{
    LTrace("Abort task: ", task);

    // If the task exists then set the destroyed flag.
    // It will be cleaned up during the next run() iteration.
    if (exists(task)) {
        LTrace("Abort managed task: ", task);
        task->_destroyed = true;
    }

    // Otherwise destroy the pointer directly (unmanaged task).
    else {
        LTrace("Delete unmanaged task: ", task);
        delete task;
    }

    return true;
}


bool TaskRunner::add(Task* task)
{
    LTrace("Add task: ", task);
    if (!exists(task)) {
        std::lock_guard<std::mutex> guard(_mutex);
        _tasks.push_back(std::unique_ptr<Task>(task));
        onAdd(task);
        return true;
    }
    return false;
}


bool TaskRunner::remove(Task* task)
{
    LTrace("Remove task: ", task);

    std::lock_guard<std::mutex> guard(_mutex);
    if (task == _current) {
        // The worker thread is executing this task; deleting it now would
        // be a use-after-free. Flag it so run() deletes it on completion.
        task->_destroyed = true;
        onRemove(task);
        return true;
    }
    for (auto it = _tasks.begin(); it != _tasks.end(); ++it) {
        if (it->get() == task) {
            _tasks.erase(it); // unique_ptr destructs the task
            onRemove(task);
            return true;
        }
    }
    return false;
}


bool TaskRunner::exists(Task* task) const
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (task == _current)
        return true;
    for (const auto& t : _tasks) {
        if (t.get() == task)
            return true;
    }
    return false;
}


Task* TaskRunner::get(uint32_t id) const
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (_current && _current->id() == id)
        return _current;
    for (const auto& t : _tasks) {
        if (t->id() == id)
            return t.get();
    }
    return nullptr;
}


Task* TaskRunner::next() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (const auto& t : _tasks) {
        if (!t->cancelled())
            return t.get();
    }
    return nullptr;
}


void TaskRunner::clear()
{
    std::lock_guard<std::mutex> guard(_mutex);
    // A task mid-execution is owned by run(); flag it so the worker deletes
    // it on completion instead of re-queuing it.
    if (_current)
        _current->_destroyed = true;
    _tasks.clear(); // unique_ptrs auto-delete all tasks
}


void TaskRunner::setRunner(std::shared_ptr<Runner> runner)
{
    LTrace("Set async: ", runner.get());

    std::lock_guard<std::mutex> guard(_mutex);
    if (_runner)
        throw std::logic_error("TaskRunner already has a runner");
    _runner = runner;
    _runner->setRepeating(true);
    _runner->start([this]() { run(); });
}


void TaskRunner::run()
{
    // Extract the next runnable task so concurrent remove()/clear() calls
    // cannot delete it while it executes; re-queue it at the back when done
    // (which also rotates the queue fairly).
    std::unique_ptr<Task> task;
    {
        std::lock_guard<std::mutex> guard(_mutex);
        for (auto it = _tasks.begin(); it != _tasks.end(); ++it) {
            if (!(*it)->cancelled()) {
                task = std::move(*it);
                _tasks.erase(it);
                _current = task.get();
                break;
            }
        }
    }

    // Run the task
    if (task) {
        LTrace("Run task: ", task.get());
        task->run();

        onRun(task.get());

        // Cancel the task if not repeating
        if (!task->repeating())
            task->cancel();

        std::lock_guard<std::mutex> guard(_mutex);
        _current = nullptr;
        if (task->destroyed()) {
            LTrace("Destroy task: ", task.get());
            // unique_ptr deletes the task when it goes out of scope
        } else {
            _tasks.push_back(std::move(task));
        }
    }

    // Dispatch the Idle signal
    Idle.emit();

    // Prevent 100% CPU
    std::this_thread::yield();
}


void TaskRunner::onAdd(Task*)
{
}


void TaskRunner::onStart(Task*)
{
}


void TaskRunner::onCancel(Task*)
{
}


void TaskRunner::onRemove(Task*)
{
}


void TaskRunner::onRun(Task*)
{
}


TaskRunner& TaskRunner::getDefault()
{
    static TaskRunner instance;
    return instance;
}


} // namespace icy


/// @}
