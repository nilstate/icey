# Sched Module

Task scheduling for deferred and recurring work, with JSON-backed persistence and trigger reconstruction.

- Namespace: `icy::sched`
- CMake target: `icey::sched`
- Primary headers: `include/icy/sched/scheduler.h`, `task.h`, `trigger.h`, `taskfactory.h`
- Directory layout: `include/` for the public API, `src/` for scheduler and trigger logic, `tests/` for timing and persistence coverage
- Trigger types cover one-shot, interval, and daily schedules

Read next:
- [Sched module guide](../../doc/modules/sched.md)
- [JSON module guide](../../doc/modules/json.md)
