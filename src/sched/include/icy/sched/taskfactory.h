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

    std::unique_ptr<sched::Task> createTask(const std::string& type /*, Scheduler& scheduler*/)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _tasks.find(type);
        if (it == _tasks.end())
            throw std::runtime_error("Failed to create scheduled task: " + type);
        return it->second();
    }

    template <typename T>
    void registerTask(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _tasks[type] = &instantiateTask<T>;
    }

    void unregisterTask(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _tasks.find(type);
        if (it == _tasks.end())
            return;
        _tasks.erase(it);
    }

    TaskMap tasks() const
    {
        std::lock_guard<std::mutex> guard(_mutex);
        return _tasks;
    }

    //
    /// Schedule Triggers

    using TriggerMap = std::map<std::string, std::unique_ptr<sched::Trigger> (*)()>;

    std::unique_ptr<sched::Trigger> createTrigger(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _triggers.find(type);
        if (it == _triggers.end())
            throw std::runtime_error("Failed to create scheduled trigger: " + type);
        return it->second();
    }

    template <typename T>
    void registerTrigger(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _triggers[type] = &instantiateTrigger<T>;
    }

    void unregisterTrigger(const std::string& type)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _triggers.find(type);
        if (it == _triggers.end())
            return;
        _triggers.erase(it);
    }

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


/// @\}
