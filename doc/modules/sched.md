# Sched

> Task scheduler for deferred and periodic jobs

**[API Reference →](../api/sched.md)** · **[Source →](../../src/sched/)**

## Overview

The `sched` module is a task scheduler for deferred and recurring jobs. We use it when work must happen at a specific time, on a repeating interval, or once per day at a configured hour; situations where a plain `icy::Task` is insufficient because we need timing control rather than just concurrent execution.

The module builds on `icy::Task` and `icy::TaskRunner` from the `base` module, adding a trigger system that decides when each task fires, and a factory for reconstructing tasks and triggers from serialized JSON. The entire scheduler state can be persisted to disk and restored on the next startup without losing pending tasks.

**Dependencies:** `base`, `json`

## Architecture

Three interacting classes form the core of the module:

- `Trigger` determines when a task runs. Each task owns one trigger, which advances `scheduleAt` after each firing and signals the scheduler when the task has expired.
- `Task` is the unit of work. We subclass it to implement the actual job. It holds a `unique_ptr<Trigger>` and a back-pointer to its `Scheduler`.
- `Scheduler` owns and runs a collection of tasks. It sorts them by `remaining()` time and dispatches them when their trigger fires. `TaskFactory` supports the scheduler by reconstructing tasks and triggers from JSON type name strings, enabling persistence.

```text
Scheduler
  └── sched::Task  (one per scheduled job)
        └── sched::Trigger  (one per task; determines when to fire)

TaskFactory  (singleton registry; type name → constructor)
```

## Usage

### Defining a task

Subclass `sched::Task`, pass a type string and a display name to the base constructor, and implement `run()`.

```cpp
#include <icy/sched/scheduler.h>

class SyncTask : public icy::sched::Task
{
public:
    SyncTask() : Task("SyncTask", "Remote Sync") {}

protected:
    void run() override
    {
        // perform the scheduled work here
    }
};
```

The type string (`"SyncTask"`) must match the name registered with `TaskFactory` if the schedule is to survive a restart.

### Scheduling with an interval trigger

`IntervalTrigger` fires repeatedly at a fixed `Timespan`. Set `maxTimes` to limit the number of firings; leave it at 0 for unlimited repetition.

```cpp
#include <icy/sched/scheduler.h>

// Run every 30 minutes, indefinitely.
auto* task = new SyncTask();
auto* trigger = task->createTrigger<icy::sched::IntervalTrigger>();
trigger->interval = Timespan(0, 0, 30, 0, 0); // days, hours, mins, secs, usecs

icy::sched::Scheduler::getDefault().schedule(task);
```

`schedule()` takes ownership of the raw pointer. We must not delete the task after scheduling it.

### Scheduling a one-shot task

`OnceOnlyTrigger` fires once at `scheduleAt` and then marks itself expired. `scheduleAt` defaults to the current time, so the task runs on the scheduler's next iteration.

```cpp
auto* task = new SyncTask();
task->createTrigger<icy::sched::OnceOnlyTrigger>();

icy::sched::Scheduler::getDefault().schedule(task);
```

To defer it: set `trigger->scheduleAt` to a future `DateTime` before calling `schedule()`.

### Scheduling a daily task with weekday exclusions

`DailyTrigger` fires once per day at a configured time-of-day. Individual days of the week can be excluded.

```cpp
auto* task = new SyncTask();
auto* trigger = task->createTrigger<icy::sched::DailyTrigger>();

// Fire at 03:00 every weekday.
trigger->timeOfDay.assign(2026, 1, 1, 3, 0, 0); // only H:M:S matters
trigger->daysExcluded = { icy::sched::Saturday, icy::sched::Sunday };

icy::sched::Scheduler::getDefault().schedule(task);
```

`DailyTrigger::update()` advances `scheduleAt` past any excluded days automatically.

### Querying remaining time

We can ask how many milliseconds remain before a task fires without touching the scheduler directly.

```cpp
std::int64_t ms = task->remaining();
std::cout << "fires in " << ms << " ms\n";
```

`remaining()` delegates to the task's trigger, so the trigger must be set before calling it.

### Cancelling tasks

```cpp
// Cancel a specific task (removes and destroys it).
icy::sched::Scheduler::getDefault().cancel(task);

// Cancel all tasks.
icy::sched::Scheduler::getDefault().clear();
```

### Persisting and restoring the schedule

Register all task and trigger types with `TaskFactory` first; the scheduler uses those registrations to reconstruct instances from JSON during `deserialize()`.

```cpp
// At startup, register types.
auto& factory = icy::sched::Scheduler::factory();
factory.registerTask<SyncTask>("SyncTask");
factory.registerTrigger<icy::sched::IntervalTrigger>("IntervalTrigger");
factory.registerTrigger<icy::sched::OnceOnlyTrigger>("OnceOnlyTrigger");
factory.registerTrigger<icy::sched::DailyTrigger>("DailyTrigger");

// Save the current schedule to disk.
json::Value root;
icy::sched::Scheduler::getDefault().serialize(root);
icy::json::saveFile("schedule.json", root);

// Restore on next startup (after registering types).
json::Value saved;
icy::json::loadFile("schedule.json", saved);
icy::sched::Scheduler::getDefault().deserialize(saved);
```

Entries that fail to deserialize are skipped and logged; the remaining tasks are still loaded.

### Printing scheduler state

```cpp
icy::sched::Scheduler::getDefault().print(std::cout);
```

Writes a pretty-printed JSON representation of all scheduled tasks and their triggers to the given stream.

### Using an independent scheduler instance

The default singleton is convenient for simple applications. For more complex cases we can construct an independent `Scheduler`:

```cpp
icy::sched::Scheduler scheduler;

auto* task = new SyncTask();
task->createTrigger<icy::sched::IntervalTrigger>()->interval = Timespan(0, 1, 0, 0, 0);
scheduler.schedule(task);
```

## Configuration

`IntervalTrigger` serializes its interval as separate day/hour/minute/second fields. The JSON for a task scheduled every 30 minutes looks like this:

```json
{
  "type": "SyncTask",
  "name": "Remote Sync",
  "trigger": {
    "type": "IntervalTrigger",
    "name": "Interval",
    "createdAt": "2026-01-01T00:00:00Z",
    "scheduleAt": "2026-01-01T00:30:00Z",
    "lastRunAt":  "2026-01-01T00:00:00Z",
    "timesRun": 0,
    "interval": {
      "days": 0,
      "hours": 0,
      "minutes": 30,
      "seconds": 0
    }
  }
}
```

`OnceOnlyTrigger` and `DailyTrigger` use the same base fields (`type`, `name`, `createdAt`, `scheduleAt`, `lastRunAt`, `timesRun`) but do not add interval fields. `DailyTrigger` does not currently serialize `daysExcluded`; that must be reconfigured at startup.

## See Also

- [JSON](json.md) — `ISerializable` interface and file I/O used by the scheduler
- [Base](base.md) — `icy::Task`, `icy::TaskRunner`, `DateTime`, `Timespan`
