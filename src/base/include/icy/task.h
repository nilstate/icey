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
#include "icy/runner.h"
#include "icy/signal.h"

#include <deque>
#include <memory>


namespace icy {


class Base_API TaskRunner;


/// Abstract base class for implementing asynchronous tasks.
///
/// Tasks are designed to be run by a TaskRunner.
class Base_API Task : public basic::Runnable
{
public:
    /// @param repeat If true, the `TaskRunner` will call `run()` repeatedly;
    ///               if false, the task is cancelled after one execution.
    Task(bool repeat = false);

    /// Sets the task to destroyed state.
    virtual void destroy();

    /// Signals that the task should be disposed of.
    virtual bool destroyed() const;

    /// Signals that the task should be called
    /// repeatedly by the TaskRunner.
    /// If this returns false the task will be cancelled.
    virtual bool repeating() const;

    /// Unique task ID.
    virtual uint32_t id() const;

    virtual ~Task();

    // Inherits basic::Runnable:
    //
    // virtual void run();
    // virtual void cancel();
    // virtual bool cancelled() const;

    Task(const Task& task) = delete;
    Task& operator=(Task const&) = delete;
    Task(Task&&) = delete;
    Task& operator=(Task&&) = delete;

protected:

    /// Called by the TaskRunner to run the task.
    /// Override this method to implement task action.
    /// Returning true means the task should be called again,
    /// and false will cause the task to be destroyed.
    /// The task will similarly be destroyed if destroy()
    /// was called during the current task iteration.
    void run() override = 0;

    /// Tasks belong to a TaskRunner instance.
    friend class TaskRunner;

    uint32_t _id;
    bool _repeating;
    bool _destroyed;
};


/// Runner for tasks that inherit the `Task` interface.
///
/// The `TaskRunner` continually loops through each task in
/// the task list calling the task's `run()` method.
///
/// The `TaskRunner` is powered by an abstract `Runner` instance, which means
/// that tasks can be executed in a thread or event loop context.
///
class Base_API TaskRunner : public basic::Runnable
{
public:
    /// @param runner Async runner to drive task execution; defaults to a new `Thread`.
    TaskRunner(std::shared_ptr<Runner> runner = nullptr);
    virtual ~TaskRunner();

    TaskRunner(const TaskRunner&) = delete;
    TaskRunner& operator=(const TaskRunner&) = delete;
    TaskRunner(TaskRunner&&) = delete;
    TaskRunner& operator=(TaskRunner&&) = delete;

    /// Starts a task, adding it if it doesn't exist.
    virtual bool start(Task* task);

    using basic::Runnable::cancel; // inherit cancel(bool) from Runnable

    /// Cancels a task.
    ///
    /// The task reference will be managed by the TaskRunner
    /// until the task is destroyed.
    virtual bool cancel(Task* task);

    /// Queues a task for destruction.
    virtual bool destroy(Task* task);

    /// Returns whether a task exists.
    virtual bool exists(Task* task) const;

    /// Returns the task pointer matching the given ID,
    /// or nullptr if no task exists.
    virtual Task* get(uint32_t id) const;

    /// Set the asynchronous context for packet processing.
    /// This may be a Thread or another derivative of Async.
    /// Must be set before the stream is activated.
    virtual void setRunner(std::shared_ptr<Runner> runner);

    /// Returns the default `TaskRunner` singleton, although
    /// TaskRunner instances may be initialized individually.
    /// The default runner should be kept for short running
    /// tasks such as timers in order to maintain performance.
    static TaskRunner& getDefault();

    /// Fires after completing an iteration of all tasks.
    NullSignal Idle;

    /// Signals when the `TaskRunner` is shutting down.
    NullSignal Shutdown;

    virtual const char* className() const { return "TaskRunner"; }

protected:
    /// Called by the async context to run the next task.
    void run() override;

    /// Adds a task to the runner.
    virtual bool add(Task* task);

    /// Removes a task from the runner.
    virtual bool remove(Task* task);

    /// Returns the next task to be run.
    virtual Task* next() const;

    /// Destroys and clears all manages tasks.
    virtual void clear();

    /// Called after a task is added.
    virtual void onAdd(Task* task);

    /// Called after a task is started.
    virtual void onStart(Task* task);

    /// Called after a task is cancelled.
    virtual void onCancel(Task* task);

    /// Called after a task is removed.
    virtual void onRemove(Task* task);

    /// Called after a task has run.
    virtual void onRun(Task* task);

protected:
    using TaskList = std::deque<std::unique_ptr<Task>>;

    mutable std::mutex _mutex;
    std::shared_ptr<Runner> _runner;
    TaskList _tasks;
};


} // namespace icy


/// @}
