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
#include "icy/sched/sched.h"
#include "icy/sched/trigger.h"
#include "icy/task.h"

#include <memory>


namespace icy {
/// Deferred and periodic job scheduling primitives.
namespace sched {


class Sched_API Scheduler;


/// Scheduled task with an attached trigger and scheduler association.
class Sched_API Task : public icy::Task
    , public json::ISerializable
{
public:
    /// Constructs a detached task without an associated scheduler.
    /// A trigger must be set before scheduling.
    /// @param type Registered type name used by TaskFactory.
    /// @param name Human-readable display name.
    Task(const std::string& type = "", const std::string& name = "");

    /// Constructs a task associated with the given scheduler.
    /// @param scheduler Scheduler that will own and run this task.
    /// @param type Registered type name used by TaskFactory.
    /// @param name Human-readable display name.
    Task(Scheduler& scheduler, const std::string& type, const std::string& name = "");

    // virtual void start();

    /// Serializes the task to JSON.
    virtual void serialize(json::Value& root) override;

    /// Deserializes the task from JSON.
    virtual void deserialize(json::Value& root) override;

    /// Creates a trigger of type T, attaches it to this task, and returns a raw pointer to it.
    /// Ownership of the trigger is transferred to this task.
    /// @tparam T A concrete subclass of sched::Trigger.
    /// @return Raw pointer to the newly created trigger (still owned by this task).
    template <typename T>
    T* createTrigger()
    {
        auto p = std::make_unique<T>();
        auto* raw = p.get();
        setTrigger(std::move(p));
        return raw;
    }

    /// Replaces the current trigger with @p trigger.
    /// @param trigger Owning pointer to the new trigger; must not be null before calling.
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

    /// Returns the registered type string for this task.
    std::string type() const;

    /// Returns the human-readable display name of this task.
    std::string name() const;

    /// Sets the human-readable display name.
    /// @param name New display name.
    void setName(const std::string& name);

    virtual ~Task();

protected:
    virtual bool beforeRun();
    virtual void run() = 0;
    virtual bool afterRun();

    // For stl::sort operations
    static bool CompareTimeout(const icy::Task* l, const icy::Task* r)
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


using TaskList = std::vector<sched::Task*>; ///< Ordered list of task pointers used by the scheduler.


} // namespace sched
} // namespace icy


/// @}
