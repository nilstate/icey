///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#include "scy/sched/scheduler.h"
#include "scy/datetime.h"
#include "scy/logger.h"
#include "scy/platform.h"

#include <algorithm>
#include <stdexcept>


using namespace std;


namespace scy {
namespace sched {


Scheduler::Scheduler()
{
}


Scheduler::~Scheduler()
{
}


void Scheduler::schedule(sched::Task* task)
{
    TaskRunner::start(task);
}


void Scheduler::cancel(sched::Task* task)
{
    TaskRunner::cancel(task);
}


void Scheduler::clear()
{
    TaskRunner::clear();
}


void Scheduler::run()
{
    // Update and sort the task list bringing the
    // next scheduled task to the front of the list.
    update();

    auto task = static_cast<sched::Task*>(next());

    // Run the task
    if (task && task->trigger().timeout()) {
#if _DEBUG
        {
            DateTime now;
            Timespan remaining = task->trigger().scheduleAt - now;
            STrace << "Waiting: "
                   << "\n\tPID: " << task
                   << "\n\tDays: " << remaining.days()
                   << "\n\tHours: " << remaining.totalHours()
                   << "\n\tMinutes: " << remaining.totalMinutes()
                   << "\n\tSeconds: " << remaining.totalSeconds()
                   << "\n\tMilliseconds: "
                   << remaining.totalMilliseconds() << "\n\tCurrentTime: "
                   << DateTimeFormatter::format(
                          now, DateTimeFormat::ISO8601_FORMAT)
                   << "\n\tScheduledAt: "
                   << DateTimeFormatter::format(
                          task->trigger().scheduleAt,
                          DateTimeFormat::ISO8601_FORMAT)
                   << endl;
        }
#endif

        // The task list may have changed during the timeout
        // duration, or the current task deleted, so we need
        // to ensure that the next pending task matches the
        // current pending task.
        if (task->beforeRun()) {
#if _DEBUG
            {
                DateTime now;
                STrace
                    << "Running: "
                    << "\n\tPID: " << task << "\n\tCurrentTime: "
                    << DateTimeFormatter::format(now,
                                                 DateTimeFormat::ISO8601_FORMAT)
                    << "\n\tScheduledTime: "
                    << DateTimeFormatter::format(task->trigger().scheduleAt,
                                                 DateTimeFormat::ISO8601_FORMAT)
                    << endl;
            }
#else
            LTrace("Running: ", task);
#endif
            task->run();
            if (task->afterRun())
                onRun(task);
            else {
                LTrace("Destroy After Run: ", task);
                task->_destroyed = true;
            }
        } else
            LTrace("Skipping Task: ", task);

        // Destroy the task if needed.
        // remove() erases the unique_ptr which deletes the task.
        if (task->destroyed()) {
            LTrace("Destroy Task: ", task);
            if (!remove(task))
                throw std::runtime_error("Failed to remove destroyed task");
        }
    }

    // Prevent 100% CPU
    scy::sleep(3);
}


void Scheduler::update()
{
    std::lock_guard<std::mutex> guard(_mutex);

    // Update and clean the task list
    auto it = _tasks.begin();
    while (it != _tasks.end()) {
        auto* task = static_cast<sched::Task*>(it->get());
        if (task->destroyed()) {
            onRemove(task);
            LTrace("Destroy: ", task);
            it = _tasks.erase(it); // unique_ptr auto-deletes the task
        } else
            ++it;
    }

    // Sort the task list so the next task to
    // trigger is at the front of the queue.
    sort(_tasks.begin(), _tasks.end(),
        [](const std::unique_ptr<scy::Task>& l, const std::unique_ptr<scy::Task>& r) {
            return sched::Task::CompareTimeout(l.get(), r.get());
        });
}


void Scheduler::serialize(json::value& root)
{
    LTrace("Serializing");

    std::lock_guard<std::mutex> guard(_mutex);
    for (const auto& t : _tasks) {
        auto* task = static_cast<sched::Task*>(t.get());
        LTrace("Serializing: ", task);
        json::value& entry = root[root.size()];
        task->serialize(entry);
        task->trigger().serialize(entry["trigger"]);
    }
}


void Scheduler::deserialize(json::value& root)
{
    LTrace("Deserializing");

    for (auto it = root.begin(); it != root.end(); it++) {
        try {
            json::assertMember(*it, "trigger");
            auto task = factory().createTask((*it)["type"].get<std::string>());
            task->deserialize((*it));
            auto trigger =
                factory().createTrigger((*it)["trigger"]["type"].get<std::string>());
            trigger->deserialize((*it)["trigger"]);
            task->setTrigger(std::move(trigger));
            schedule(task.release()); // transfers ownership to TaskRunner
        } catch (std::exception& exc) {
            LError("Deserialization Error: ", exc.what());
        }
    }
}


void Scheduler::print(std::ostream& ost)
{
    json::value data;
    serialize(data);
    ost << data.dump(4);
}


Scheduler& Scheduler::getDefault()
{
    static Scheduler instance;
    return instance;
}


TaskFactory& Scheduler::factory()
{
    return TaskFactory::getDefault();
}


} // namespace sched
} // namespace scy


/// @\}
