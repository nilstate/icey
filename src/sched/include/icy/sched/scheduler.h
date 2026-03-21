///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#pragma once


#include "icy/json/iserializable.h"
#include "icy/logger.h"
#include "icy/sched/sched.h"
#include "icy/sched/task.h"
#include "icy/sched/taskfactory.h"
#include "icy/task.h"

#include <vector>


namespace icy {
namespace sched {


/// The Scheduler manages and runs tasks
/// that need to be executed at specific times.
class Sched_API Scheduler : public TaskRunner
    , public json::ISerializable
{
public:
    Scheduler();
    virtual ~Scheduler();

    /// Adds @p task to the scheduler and starts running it on its configured trigger.
    /// The scheduler takes ownership of the task.
    /// @param task Task to schedule; must have a trigger set.
    virtual void schedule(sched::Task* task);

    using TaskRunner::cancel; // inherit cancel(Task*) and cancel(bool)

    /// Removes @p task from the scheduler and cancels any pending execution.
    /// @param task Task to cancel.
    virtual void cancel(sched::Task* task);

    /// Removes all scheduled tasks.
    void clear() override;

    /// Serializes all scheduled tasks and their triggers to @p root.
    /// @param root JSON array to append serialized task entries to.
    virtual void serialize(json::Value& root) override;

    /// Reconstructs the task list from @p root using the TaskFactory.
    /// Skips entries that fail to deserialize and logs the error.
    /// @param root JSON array previously produced by serialize().
    virtual void deserialize(json::Value& root) override;

    /// Writes a pretty-printed JSON representation of all tasks to @p ost.
    /// @param ost Output stream to write to.
    virtual void print(std::ostream& ost);

    /// Returns the default Scheduler singleton,
    /// although Scheduler instances may also be
    /// initialized individually.
    static Scheduler& getDefault();

    /// Returns the TaskFactory singleton.
    static sched::TaskFactory& factory();

protected:
    virtual void run() override;
    virtual void update();
};


// static std::string DepreciatedDateFormat = "%Y-%m-%d %H:%M:%S %Z";


} // namespace sched
} // namespace icy


/// @\}
