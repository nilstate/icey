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
#include "scy/sched/sched.h"
#include "scy/sched/trigger.h"
#include "scy/task.h"

#include <memory>


namespace scy {
namespace sched {


class Sched_API Scheduler;


/// This class extends the Task class to implement
/// scheduling capabilities.
class Sched_API Task : public scy::Task
    , public json::ISerializable
{
public:
    Task(const std::string& type = "", const std::string& name = "");
    Task(Scheduler& scheduler, const std::string& type, const std::string& name = "");

    // virtual void start();

    /// Serializes the task to JSON.
    virtual void serialize(json::Value& root) override;

    /// Deserializes the task from JSON.
    virtual void deserialize(json::Value& root) override;

    template <typename T>
    T* createTrigger()
    {
        auto p = std::make_unique<T>();
        auto* raw = p.get();
        setTrigger(std::move(p));
        return raw;
    }

    void setTrigger(std::unique_ptr<sched::Trigger> trigger);

    /// Returns a reference to the associated
    /// sched::Trigger or throws an exception.
    sched::Trigger& trigger();

    /// Returns a reference to the associated
    /// Scheduler or throws an exception.
    Scheduler& scheduler();

    /// Returns the milliseconds remaining
    /// until the next scheduled timeout.
    /// An sched::Trigger must be associated
    /// or an exception will be thrown.
    std::int64_t remaining() const;

    std::string type() const;
    std::string name() const;
    void setName(const std::string& name);

    virtual ~Task();

protected:
    virtual bool beforeRun();
    virtual void run() = 0;
    virtual bool afterRun();

    // For stl::sort operations
    static bool CompareTimeout(const scy::Task* l, const scy::Task* r)
    {
        return reinterpret_cast<const Task*>(l)->remaining() <
               reinterpret_cast<const Task*>(r)->remaining();
    }

    friend class Scheduler;

    std::string _type;
    std::string _name;
    sched::Scheduler* _scheduler;
    std::unique_ptr<sched::Trigger> _trigger;
    mutable std::mutex _mutex;
};


using TaskList = std::vector<sched::Task*>;


} // namespace sched
} // namespace scy


/// @\}
