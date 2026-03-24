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


#include "icy/logger.h"
#include "icy/sched/sched.h"
#include "icy/sched/task.h"
#include "icy/sched/trigger.h"

#include <memory>
#include <vector>


namespace icy {
namespace sched {


class Sched_API Scheduler;


template <typename T>
std::unique_ptr<sched::Task> instantiateTask()
{
    return std::make_unique<T>();
}

template <typename T>
std::unique_ptr<sched::Trigger> instantiateTrigger()
{
    return std::make_unique<T>();
}


/// The TaskFactory can dynamically instantiate
/// registered sched::Task and sched::Trigger
/// classes from named strings.
class Sched_API TaskFactory
{
public:
    /// Returns the default TaskFactory singleton.
    static TaskFactory& getDefault()
    {
        static TaskFactory instance;
        return instance;
    }

    /// Scheduled Tasks

    using TaskMap = std::map<std::string, std::unique_ptr<sched::Task> (*)(/*Scheduler&*/)>;

    /// Instantiates and returns a registered task by type name.
    /// @param type Registered type name.
    /// @return Owning pointer to the new task instance.
    /// @throws std::runtime_error if @p type is not registered.
    std::unique_ptr<sched::Task> createTask(const std::string& type /*, Scheduler& scheduler*/)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _tasks.find(type);
        if (it == _tasks.end())
            throw std::runtime_error("Failed to create scheduled task: " + type);
        return it->second();
    }

    /// Registers a task type T under the given name.
    /// Subsequent calls to createTask() with this @p type will return a T instance.
    /// @tparam T Concrete subclass of sched::Task with a default constructor.
    /// @param type Type name string to register.
    template <typename T>
    void registerTask(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _tasks[type] = &instantiateTask<T>;
    }

    /// Removes the task registration for @p type. No-op if not registered.
    /// @param type Type name to deregister.
    void unregisterTask(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _tasks.find(type);
        if (it == _tasks.end())
            return;
        _tasks.erase(it);
    }

    /// Returns a snapshot copy of the registered task map.
    TaskMap tasks() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _tasks;
    }

    //
    /// Schedule Triggers

    using TriggerMap = std::map<std::string, std::unique_ptr<sched::Trigger> (*)()>;

    /// Instantiates and returns a registered trigger by type name.
    /// @param type Registered type name.
    /// @return Owning pointer to the new trigger instance.
    /// @throws std::runtime_error if @p type is not registered.
    std::unique_ptr<sched::Trigger> createTrigger(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _triggers.find(type);
        if (it == _triggers.end())
            throw std::runtime_error("Failed to create scheduled trigger: " + type);
        return it->second();
    }

    /// Registers a trigger type T under the given name.
    /// @tparam T Concrete subclass of sched::Trigger with a default constructor.
    /// @param type Type name string to register.
    template <typename T>
    void registerTrigger(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _triggers[type] = &instantiateTrigger<T>;
    }

    /// Removes the trigger registration for @p type. No-op if not registered.
    /// @param type Type name to deregister.
    void unregisterTrigger(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _triggers.find(type);
        if (it == _triggers.end())
            return;
        _triggers.erase(it);
    }

    /// Returns a snapshot copy of the registered trigger map.
    TriggerMap triggers() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _triggers;
    }

protected:
    mutable std::mutex _mutex;

    TaskMap _tasks;
    TriggerMap _triggers;
};


} // namespace sched
} // namespace icy


/// @}
