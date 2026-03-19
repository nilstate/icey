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

    virtual void schedule(sched::Task* task);
    using TaskRunner::cancel; // inherit cancel(Task*) and cancel(bool)
    virtual void cancel(sched::Task* task);
    void clear() override;

    virtual void serialize(json::Value& root) override;
    virtual void deserialize(json::Value& root) override;

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
