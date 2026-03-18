///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#pragma once


#include "scy/json/iserializable.h"
#include "scy/logger.h"
#include "scy/sched/sched.h"
#include "scy/sched/task.h"
#include "scy/sched/taskfactory.h"
#include "scy/task.h"

#include <vector>


namespace scy {
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
} // namespace scy


/// @\}
