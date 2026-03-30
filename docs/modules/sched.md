# Sched

> Task scheduler for deferred and periodic jobs

**[API Reference →](../api/sched.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/sched/)**

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

## Semantics

The scheduler is intentionally small, so its timing rules should be explicit:

- `Scheduler::schedule()` transfers ownership immediately and assigns the task's scheduler back-pointer before the task starts running.
- `OnceOnlyTrigger` fires at `scheduleAt`. If `scheduleAt` is left at its default current time, the task runs on the next scheduler iteration.
- `IntervalTrigger` does not fire immediately by default. On first schedule, the scheduler normalizes the first run to `createdAt + interval` unless `scheduleAt` was explicitly overridden.
- `DailyTrigger` computes the next valid future wall-clock slot from `timeOfDay` and `daysExcluded`; it does not run immediately just because it was added after today's slot already passed.
- `deserialize()` replaces the current in-memory schedule. Invalid entries are skipped and logged; valid entries still load.
- Restore behavior is intentionally different for recurring triggers:
  - overdue once-only tasks run immediately after restore
  - overdue interval tasks skip missed backlog and resume on the next future interval boundary
  - overdue daily tasks skip stale slots and resume on the next valid future day/time

`sched` uses wall-clock time (`DateTime`) rather than a monotonic clock. That makes it appropriate for service maintenance work and persisted schedules, but it also means clock changes can affect when jobs fire.

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
auto task = std::make_unique<SyncTask>();
auto* trigger = task->createTrigger<icy::sched::IntervalTrigger>();
trigger->interval = Timespan(0, 0, 30, 0, 0); // days, hours, mins, secs, usecs

icy::sched::Scheduler::getDefault().schedule(std::move(task));
```

Passing a `std::unique_ptr` is the preferred path. The scheduler still owns the task after `schedule()` either way. The first run happens after one full interval unless `trigger->scheduleAt` was explicitly set before scheduling.

### Scheduling a one-shot task

`OnceOnlyTrigger` fires once at `scheduleAt` and then marks itself expired. `scheduleAt` defaults to the current time, so the task runs on the scheduler's next iteration.

```cpp
auto task = std::make_unique<SyncTask>();
task->createTrigger<icy::sched::OnceOnlyTrigger>();

icy::sched::Scheduler::getDefault().schedule(std::move(task));
```

To defer it: set `trigger->scheduleAt` to a future `DateTime` before calling `schedule()`.

### Scheduling a daily task with weekday exclusions

`DailyTrigger` fires once per day at a configured time-of-day. Individual days of the week can be excluded.

```cpp
auto task = std::make_unique<SyncTask>();
auto* trigger = task->createTrigger<icy::sched::DailyTrigger>();

// Fire at 03:00 every weekday.
trigger->timeOfDay.assign(2026, 1, 1, 3, 0, 0); // only H:M:S matters
trigger->daysExcluded = { icy::sched::Saturday, icy::sched::Sunday };

icy::sched::Scheduler::getDefault().schedule(std::move(task));
```

`DailyTrigger::update()` advances `scheduleAt` past any excluded days automatically.

### Querying remaining time

We can ask how many milliseconds remain before a task fires without touching the scheduler directly.

```cpp
std::int64_t ms = task->remaining();
std::cout << "fires in " << ms << " ms\n";
```

`remaining()` delegates to the task's trigger, so the trigger must be set before calling it.

### Inspecting scheduler state

```cpp
auto& scheduler = icy::sched::Scheduler::getDefault();
std::cout << "pending: " << scheduler.size() << "\n";
if (scheduler.empty())
    std::cout << "no pending work\n";
```

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

`deserialize()` replaces the current in-memory schedule with the decoded one. Entries that fail to deserialize are skipped and logged; the remaining tasks are still loaded.

Recurring triggers are normalized on restore:

- `IntervalTrigger` preserves cadence and advances `scheduleAt` to the first future interval boundary instead of waiting a full new interval from restore time.
- `DailyTrigger` resumes at the next valid future daily slot based on `timeOfDay` and `daysExcluded`.
- `OnceOnlyTrigger` keeps its serialized `scheduleAt`, so an overdue one-shot runs immediately after restore.

### Printing scheduler state

```cpp
icy::sched::Scheduler::getDefault().print(std::cout);
```

Writes a pretty-printed JSON representation of all scheduled tasks and their triggers to the given stream.

### Using an independent scheduler instance

The default singleton is convenient for simple applications. For more complex cases we can construct an independent `Scheduler`:

```cpp
icy::sched::Scheduler scheduler;

auto task = std::make_unique<SyncTask>();
task->createTrigger<icy::sched::IntervalTrigger>()->interval = Timespan(0, 1, 0, 0, 0);
scheduler.schedule(std::move(task));
```

## Configuration

`IntervalTrigger` serializes its interval as separate day/hour/minute/second fields plus `maxTimes`. The JSON for a task scheduled every 30 minutes looks like this:

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
    "maxTimes": 0,
    "interval": {
      "days": 0,
      "hours": 0,
      "minutes": 30,
      "seconds": 0
    }
  }
}
```

`OnceOnlyTrigger` uses only the base fields (`type`, `name`, `createdAt`, `scheduleAt`, `lastRunAt`, `timesRun`).

`DailyTrigger` adds `timeOfDay` and `daysExcluded`, so its configuration now round-trips fully:

```json
{
  "type": "SyncTask",
  "name": "Remote Sync",
  "trigger": {
    "type": "DailyTrigger",
    "name": "Daily",
    "createdAt": "2026-01-01T00:00:00Z",
    "scheduleAt": "2026-01-02T03:00:00Z",
    "lastRunAt": "2026-01-01T03:00:00Z",
    "timesRun": 1,
    "timeOfDay": "2026-01-01T03:00:00Z",
    "daysExcluded": [0, 6]
  }
}
```

## Service Patterns

These are the kinds of jobs `sched` is best at inside long-running services:

### Retention cleanup

Use a daily trigger to prune old recordings, snapshots, or temporary files during a quiet maintenance window.

### Periodic health work

Use an interval trigger for things like metrics flushes, heartbeat snapshots, or periodic cache refreshes. Set `maxTimes = 0` to keep the job running indefinitely.

### Snapshot and clip housekeeping

Use one-shot triggers for delayed cleanup that should happen after an artifact is created, such as deleting a temporary preview file ten minutes later.

### Retry/backoff orchestration

Wrap a retry attempt in a task and schedule the next attempt explicitly with a one-shot trigger. `sched` is not a cron engine, but it is a good fit for bounded service-side retry loops where the retry state lives in the task itself.

## See Also

- [JSON](json.md) — `ISerializable` interface and file I/O used by the scheduler
- [Base](base.md) — `icy::Task`, `icy::TaskRunner`, `DateTime`, `Timespan`
