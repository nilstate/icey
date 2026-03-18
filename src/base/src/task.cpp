///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/task.h"
#include "scy/logger.h"
#include "scy/platform.h"
#include "scy/util.h"

#include <iostream>
#include <thread>




namespace scy {


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
    if (_runner)
        _runner->cancel();
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
    for (const auto& t : _tasks) {
        if (t.get() == task)
            return true;
    }
    return false;
}


Task* TaskRunner::get(uint32_t id) const
{
    std::lock_guard<std::mutex> guard(_mutex);
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
    Task* task = next();

    // Run the task
    if (task) {
        // Check once more that the task has not been cancelled
        if (!task->cancelled()) {
            LTrace("Run task: ", task);
            task->run();

            onRun(task);

            // Cancel the task if not repeating
            if (!task->repeating())
                task->cancel();
        }

        // Advance the task queue (rotate front to back)
        {
            std::lock_guard<std::mutex> guard(_mutex);
            auto t = std::move(_tasks.front());
            _tasks.pop_front();
            _tasks.push_back(std::move(t));
        }

        // Destroy the task if required.
        // remove() will erase the unique_ptr, which deletes the task.
        if (task->destroyed()) {
            LTrace("Destroy task: ", task);
            remove(task);
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


} // namespace scy


/// @\}
