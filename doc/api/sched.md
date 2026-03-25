{#schedmodule}

# sched

Task scheduler for deferred and periodic jobs.

### Namespaces

| Name | Description |
|------|-------------|
| [`sched`](#sched) |  |

{#sched}

# sched

### Classes

| Name | Description |
|------|-------------|
| [`Scheduler`](#scheduler) | The [Scheduler](#scheduler) manages and runs tasks that need to be executed at specific times. |
| [`Task`](#task-1) | This class extends the [Task](#task-1) class to implement scheduling capabilities. |
| [`TaskFactory`](#taskfactory) | The [TaskFactory](#taskfactory) can dynamically instantiate registered [sched::Task](#task-1) and [sched::Trigger](#trigger-1) classes from named strings. |
| [`DailyTrigger`](#dailytrigger) | [Trigger](#trigger-1) that fires once per day at a configured time, with optional day-of-week exclusions. |
| [`IntervalTrigger`](#intervaltrigger) | [Trigger](#trigger-1) that fires repeatedly at a fixed time interval. |
| [`OnceOnlyTrigger`](#onceonlytrigger) | [Trigger](#trigger-1) that fires exactly once at the scheduled time and then expires. |
| [`Trigger`](#trigger-1) | Base class for scheduled task triggers that determine when a task should run. |

### Enumerations

| Name | Description |
|------|-------------|
| [`DaysOfTheWeek`](#daysoftheweek)  | Days of the week. |
| [`MonthOfTheYeay`](#monthoftheyeay)  | Months of the year. |

---

{#daysoftheweek}

#### DaysOfTheWeek

```cpp
enum DaysOfTheWeek
```

Days of the week.

| Value | Description |
|-------|-------------|
| `Sunday` |  |
| `Monday` |  |
| `Tuesday` |  |
| `Wednesday` |  |
| `Thursday` |  |
| `Friday` |  |
| `Saturday` |  |

---

{#monthoftheyeay}

#### MonthOfTheYeay

```cpp
enum MonthOfTheYeay
```

Months of the year.

| Value | Description |
|-------|-------------|
| `January` |  |
| `February` |  |
| `March` |  |
| `April` |  |
| `May` |  |
| `June` |  |
| `July` |  |
| `August` |  |
| `September` |  |
| `October` |  |
| `November` |  |
| `December` |  |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< sched::Task * >` | [`TaskList`](#tasklist)  |  |

---

{#tasklist}

#### TaskList

```cpp
std::vector< sched::Task * > TaskList()
```

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< sched::Task >` | [`instantiateTask`](#instantiatetask)  |  |
| `std::unique_ptr< sched::Trigger >` | [`instantiateTrigger`](#instantiatetrigger)  |  |

---

{#instantiatetask}

#### instantiateTask

```cpp
template<typename T> std::unique_ptr< sched::Task > instantiateTask()
```

---

{#instantiatetrigger}

#### instantiateTrigger

```cpp
template<typename T> std::unique_ptr< sched::Trigger > instantiateTrigger()
```

{#scheduler}

## Scheduler

```cpp
#include <icy/sched/scheduler.h>
```

> **Inherits:** [`TaskRunner`](base.md#taskrunner), [`ISerializable`](json.md#iserializable)

The [Scheduler](#scheduler) manages and runs tasks that need to be executed at specific times.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Scheduler`](#scheduler-1)  |  |
| `void` | [`schedule`](#schedule) `virtual` | Adds `task` to the scheduler and starts running it on its configured trigger. The scheduler takes ownership of the task.  |
| `void` | [`cancel`](#cancel-4) `virtual` | Removes `task` from the scheduler and cancels any pending execution.  |
| `void` | [`clear`](#clear-2) `virtual` | Removes all scheduled tasks. |
| `void` | [`serialize`](#serialize-3) `virtual` | Serializes all scheduled tasks and their triggers to `root`.  |
| `void` | [`deserialize`](#deserialize-3) `virtual` | Reconstructs the task list from `root` using the [TaskFactory](#taskfactory). Skips entries that fail to deserialize and logs the error.  |
| `void` | [`print`](#print-14) `virtual` | Writes a pretty-printed JSON representation of all tasks to `ost`.  |
| `bool` | [`cancel`](#cancel-5)  | Cancels a task. |

---

{#scheduler-1}

#### Scheduler

```cpp
Scheduler()
```

---

{#schedule}

#### schedule

`virtual`

```cpp
virtual void schedule(sched::Task * task)
```

Adds `task` to the scheduler and starts running it on its configured trigger. The scheduler takes ownership of the task. 
#### Parameters
* `task` [Task](#task-1) to schedule; must have a trigger set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `[sched::Task](#task-1) *` |  |

---

{#cancel-4}

#### cancel

`virtual`

```cpp
virtual void cancel(sched::Task * task)
```

Removes `task` from the scheduler and cancels any pending execution. 
#### Parameters
* `task` [Task](#task-1) to cancel.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `[sched::Task](#task-1) *` |  |

---

{#clear-2}

#### clear

`virtual`

```cpp
virtual void clear()
```

Removes all scheduled tasks.

---

{#serialize-3}

#### serialize

`virtual`

```cpp
virtual void serialize(json::Value & root)
```

Serializes all scheduled tasks and their triggers to `root`. 
#### Parameters
* `root` JSON array to append serialized task entries to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

---

{#deserialize-3}

#### deserialize

`virtual`

```cpp
virtual void deserialize(json::Value & root)
```

Reconstructs the task list from `root` using the [TaskFactory](#taskfactory). Skips entries that fail to deserialize and logs the error. 
#### Parameters
* `root` JSON array previously produced by [serialize()](#serialize-3).

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

---

{#print-14}

#### print

`virtual`

```cpp
virtual void print(std::ostream & ost)
```

Writes a pretty-printed JSON representation of all tasks to `ost`. 
#### Parameters
* `ost` Output stream to write to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

{#cancel-5}

#### cancel

```cpp
bool cancel(Task * task)
```

Cancels a task.

The task reference will be managed the [TaskRunner](base.md#taskrunner) until the task is destroyed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `[Task](#task-1) *` |  |

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `Scheduler &` | [`getDefault`](#getdefault-2) `static` | Returns the default [Scheduler](#scheduler) singleton, although [Scheduler](#scheduler) instances may also be initialized individually. |
| `sched::TaskFactory &` | [`factory`](#factory-1) `static` | Returns the [TaskFactory](#taskfactory) singleton. |

---

{#getdefault-2}

#### getDefault

`static`

```cpp
static Scheduler & getDefault()
```

Returns the default [Scheduler](#scheduler) singleton, although [Scheduler](#scheduler) instances may also be initialized individually.

---

{#factory-1}

#### factory

`static`

```cpp
static sched::TaskFactory & factory()
```

Returns the [TaskFactory](#taskfactory) singleton.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`run`](#run-6) `virtual` | The run method will be called by the asynchronous context. |
| `void` | [`update`](#update-5) `virtual` |  |

---

{#run-6}

#### run

`virtual`

```cpp
virtual void run()
```

The run method will be called by the asynchronous context.

---

{#update-5}

#### update

`virtual`

```cpp
virtual void update()
```

{#task-1}

## Task

```cpp
#include <icy/sched/task.h>
```

> **Inherits:** [`Task`](base.md#task), [`ISerializable`](json.md#iserializable)

This class extends the [Task](#task-1) class to implement scheduling capabilities.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Task`](#task-2)  | Constructs a detached task without an associated scheduler. A trigger must be set before scheduling.  |
|  | [`Task`](#task-3)  | Constructs a task associated with the given scheduler.  |
| `void` | [`serialize`](#serialize-4) `virtual` | Serializes the task to JSON. |
| `void` | [`deserialize`](#deserialize-4) `virtual` | Deserializes the task from JSON. |
| `T *` | [`createTrigger`](#createtrigger) `inline` | Creates a trigger of type T, attaches it to this task, and returns a raw pointer to it. Ownership of the trigger is transferred to this task.  |
| `void` | [`setTrigger`](#settrigger)  | Replaces the current trigger with `trigger`.  |
| `sched::Trigger &` | [`trigger`](#trigger)  | Returns a reference to the associated [sched::Trigger](#trigger-1) or throws an exception. |
| `Scheduler &` | [`scheduler`](#scheduler-2)  | Returns a reference to the associated [Scheduler](#scheduler) or throws an exception. |
| `std::int64_t` | [`remaining`](#remaining) `const` | Returns the milliseconds remaining until the next scheduled timeout. An [sched::Trigger](#trigger-1) must be associated or an exception will be thrown. |
| `std::string` | [`type`](#type-16) `const` | Returns the registered type string for this task. |
| `std::string` | [`name`](#name-8) `const` | Returns the human-readable display name of this task. |
| `void` | [`setName`](#setname-1)  | Sets the human-readable display name.  |

---

{#task-2}

#### Task

```cpp
Task(const std::string & type, const std::string & name)
```

Constructs a detached task without an associated scheduler. A trigger must be set before scheduling. 
#### Parameters
* `type` Registered type name used by [TaskFactory](#taskfactory). 

* `name` Human-readable display name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `name` | `const std::string &` |  |

---

{#task-3}

#### Task

```cpp
Task(Scheduler & scheduler, const std::string & type, const std::string & name)
```

Constructs a task associated with the given scheduler. 
#### Parameters
* `scheduler` [Scheduler](#scheduler) that will own and run this task. 

* `type` Registered type name used by [TaskFactory](#taskfactory). 

* `name` Human-readable display name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheduler` | `[Scheduler](#scheduler) &` |  |
| `type` | `const std::string &` |  |
| `name` | `const std::string &` |  |

---

{#serialize-4}

#### serialize

`virtual`

```cpp
virtual void serialize(json::Value & root)
```

Serializes the task to JSON.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

---

{#deserialize-4}

#### deserialize

`virtual`

```cpp
virtual void deserialize(json::Value & root)
```

Deserializes the task from JSON.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

---

{#createtrigger}

#### createTrigger

`inline`

```cpp
template<typename T> inline T * createTrigger()
```

Creates a trigger of type T, attaches it to this task, and returns a raw pointer to it. Ownership of the trigger is transferred to this task. 
#### Parameters
* `T` A concrete subclass of [sched::Trigger](#trigger-1). 

#### Returns
Raw pointer to the newly created trigger (still owned by this task).

---

{#settrigger}

#### setTrigger

```cpp
void setTrigger(std::unique_ptr< sched::Trigger > trigger)
```

Replaces the current trigger with `trigger`. 
#### Parameters
* `trigger` Owning pointer to the new trigger; must not be null before calling.

| Parameter | Type | Description |
|-----------|------|-------------|
| `trigger` | `std::unique_ptr< [sched::Trigger](#trigger-1) >` |  |

---

{#trigger}

#### trigger

```cpp
sched::Trigger & trigger()
```

Returns a reference to the associated [sched::Trigger](#trigger-1) or throws an exception.

---

{#scheduler-2}

#### scheduler

```cpp
Scheduler & scheduler()
```

Returns a reference to the associated [Scheduler](#scheduler) or throws an exception.

---

{#remaining}

#### remaining

`const`

```cpp
std::int64_t remaining() const
```

Returns the milliseconds remaining until the next scheduled timeout. An [sched::Trigger](#trigger-1) must be associated or an exception will be thrown.

---

{#type-16}

#### type

`const`

```cpp
std::string type() const
```

Returns the registered type string for this task.

---

{#name-8}

#### name

`const`

```cpp
std::string name() const
```

Returns the human-readable display name of this task.

---

{#setname-1}

#### setName

```cpp
void setName(const std::string & name)
```

Sets the human-readable display name. 
#### Parameters
* `name` New display name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_type`](#_type-2)  |  |
| `std::string` | [`_name`](#_name-1)  |  |
| `sched::Scheduler *` | [`_scheduler`](#_scheduler)  |  |
| `std::unique_ptr< sched::Trigger >` | [`_trigger`](#_trigger)  |  |
| `std::mutex` | [`_mutex`](#_mutex-14)  |  |

---

{#_type-2}

#### _type

```cpp
std::string _type
```

---

{#_name-1}

#### _name

```cpp
std::string _name
```

---

{#_scheduler}

#### _scheduler

```cpp
sched::Scheduler * _scheduler
```

---

{#_trigger}

#### _trigger

```cpp
std::unique_ptr< sched::Trigger > _trigger
```

---

{#_mutex-14}

#### _mutex

```cpp
std::mutex _mutex
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`beforeRun`](#beforerun) `virtual` |  |
| `void` | [`run`](#run-7)  | Called by the [TaskRunner](base.md#taskrunner) to run the task. Override this method to implement task action. Returning true means the task should be called again, and false will cause the task to be destroyed. The task will similarly be destroyed if [destroy()](base.md#classicy_1_1Task_1a639abe7ed51fd9294d6210e1d8264814) was called during the current task iteration. |
| `bool` | [`afterRun`](#afterrun) `virtual` |  |

---

{#beforerun}

#### beforeRun

`virtual`

```cpp
virtual bool beforeRun()
```

---

{#run-7}

#### run

```cpp
void run()
```

Called by the [TaskRunner](base.md#taskrunner) to run the task. Override this method to implement task action. Returning true means the task should be called again, and false will cause the task to be destroyed. The task will similarly be destroyed if [destroy()](base.md#classicy_1_1Task_1a639abe7ed51fd9294d6210e1d8264814) was called during the current task iteration.

---

{#afterrun}

#### afterRun

`virtual`

```cpp
virtual bool afterRun()
```

{#taskfactory}

## TaskFactory

```cpp
#include <icy/sched/taskfactory.h>
```

The [TaskFactory](#taskfactory) can dynamically instantiate registered [sched::Task](#task-1) and [sched::Trigger](#trigger-1) classes from named strings.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< sched::Task >` | [`createTask`](#createtask) `inline` | Instantiates and returns a registered task by type name.  |
| `void` | [`registerTask`](#registertask) `inline` | Registers a task type T under the given name. Subsequent calls to [createTask()](#createtask) with this `type` will return a T instance.  |
| `void` | [`unregisterTask`](#unregistertask) `inline` | Removes the task registration for `type`. No-op if not registered.  |
| `TaskMap` | [`tasks`](#tasks-2) `const` `inline` | Returns a snapshot copy of the registered task map. |
| `std::unique_ptr< sched::Trigger >` | [`createTrigger`](#createtrigger-1) `inline` | Instantiates and returns a registered trigger by type name.  |
| `void` | [`registerTrigger`](#registertrigger) `inline` | Registers a trigger type T under the given name.  |
| `void` | [`unregisterTrigger`](#unregistertrigger) `inline` | Removes the trigger registration for `type`. No-op if not registered.  |
| `TriggerMap` | [`triggers`](#triggers) `const` `inline` | Returns a snapshot copy of the registered trigger map. |

---

{#createtask}

#### createTask

`inline`

```cpp
inline std::unique_ptr< sched::Task > createTask(const std::string & type)
```

Instantiates and returns a registered task by type name. 
#### Parameters
* `type` Registered type name. 

#### Returns
Owning pointer to the new task instance. 

#### Exceptions
* `std::runtime_error` if `type` is not registered.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

{#registertask}

#### registerTask

`inline`

```cpp
template<typename T> inline void registerTask(const std::string & type)
```

Registers a task type T under the given name. Subsequent calls to [createTask()](#createtask) with this `type` will return a T instance. 
#### Parameters
* `T` Concrete subclass of [sched::Task](#task-1) with a default constructor. 

#### Parameters
* `type` Type name string to register.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

{#unregistertask}

#### unregisterTask

`inline`

```cpp
inline void unregisterTask(const std::string & type)
```

Removes the task registration for `type`. No-op if not registered. 
#### Parameters
* `type` Type name to deregister.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

{#tasks-2}

#### tasks

`const` `inline`

```cpp
inline TaskMap tasks() const
```

Returns a snapshot copy of the registered task map.

---

{#createtrigger-1}

#### createTrigger

`inline`

```cpp
inline std::unique_ptr< sched::Trigger > createTrigger(const std::string & type)
```

Instantiates and returns a registered trigger by type name. 
#### Parameters
* `type` Registered type name. 

#### Returns
Owning pointer to the new trigger instance. 

#### Exceptions
* `std::runtime_error` if `type` is not registered.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

{#registertrigger}

#### registerTrigger

`inline`

```cpp
template<typename T> inline void registerTrigger(const std::string & type)
```

Registers a trigger type T under the given name. 
#### Parameters
* `T` Concrete subclass of [sched::Trigger](#trigger-1) with a default constructor. 

#### Parameters
* `type` Type name string to register.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

{#unregistertrigger}

#### unregisterTrigger

`inline`

```cpp
inline void unregisterTrigger(const std::string & type)
```

Removes the trigger registration for `type`. No-op if not registered. 
#### Parameters
* `type` Type name to deregister.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

{#triggers}

#### triggers

`const` `inline`

```cpp
inline TriggerMap triggers() const
```

Returns a snapshot copy of the registered trigger map.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `TaskFactory &` | [`getDefault`](#getdefault-3) `static` `inline` | Returns the default [TaskFactory](#taskfactory) singleton. |

---

{#getdefault-3}

#### getDefault

`static` `inline`

```cpp
static inline TaskFactory & getDefault()
```

Returns the default [TaskFactory](#taskfactory) singleton.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-15)  |  |
| `TaskMap` | [`_tasks`](#_tasks-2)  |  |
| `TriggerMap` | [`_triggers`](#_triggers)  |  |

---

{#_mutex-15}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_tasks-2}

#### _tasks

```cpp
TaskMap _tasks
```

---

{#_triggers}

#### _triggers

```cpp
TriggerMap _triggers
```

### Public Types

| Name | Description |
|------|-------------|
| [`TaskMap`](#taskmap)  | Scheduled Tasks. |
| [`TriggerMap`](#triggermap)  | Schedule Triggers. |

---

{#taskmap}

#### TaskMap

```cpp
std::map< std::string, std::unique_ptr< sched::Task >(*)()> TaskMap()
```

Scheduled Tasks.

---

{#triggermap}

#### TriggerMap

```cpp
std::map< std::string, std::unique_ptr< sched::Trigger >(*)()> TriggerMap()
```

Schedule Triggers.

{#dailytrigger}

## DailyTrigger

```cpp
#include <icy/sched/trigger.h>
```

> **Inherits:** [`Trigger`](#trigger-1)

[Trigger](#trigger-1) that fires once per day at a configured time, with optional day-of-week exclusions.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `DateTime` | [`timeOfDay`](#timeofday)  | This value represents the time of day the task will trigger. The date part of the timestamp is redundant. |
| `std::vector< DaysOfTheWeek >` | [`daysExcluded`](#daysexcluded)  | Days of the week can be excluded by adding the appropriate bit flag here. |

---

{#timeofday}

#### timeOfDay

```cpp
DateTime timeOfDay
```

This value represents the time of day the task will trigger. The date part of the timestamp is redundant.

---

{#daysexcluded}

#### daysExcluded

```cpp
std::vector< DaysOfTheWeek > daysExcluded
```

Days of the week can be excluded by adding the appropriate bit flag here.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`DailyTrigger`](#dailytrigger-1)  | Constructs the trigger with type "DailyTrigger". |
| `void` | [`update`](#update-2) `virtual` | Computes the next scheduleAt value by advancing to the next non-excluded weekday at the configured timeOfDay. |

---

{#dailytrigger-1}

#### DailyTrigger

```cpp
DailyTrigger()
```

Constructs the trigger with type "DailyTrigger".

---

{#update-2}

#### update

`virtual`

```cpp
virtual void update()
```

Computes the next scheduleAt value by advancing to the next non-excluded weekday at the configured timeOfDay.

{#intervaltrigger}

## IntervalTrigger

```cpp
#include <icy/sched/trigger.h>
```

> **Inherits:** [`Trigger`](#trigger-1)

[Trigger](#trigger-1) that fires repeatedly at a fixed time interval.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Timespan` | [`interval`](#interval)  | This value represents the interval to wait before running the task. |
| `int` | [`maxTimes`](#maxtimes)  | The maximum number of times the task will be run before it is destroyed. 0 for no effect. |

---

{#interval}

#### interval

```cpp
Timespan interval
```

This value represents the interval to wait before running the task.

---

{#maxtimes}

#### maxTimes

```cpp
int maxTimes
```

The maximum number of times the task will be run before it is destroyed. 0 for no effect.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`IntervalTrigger`](#intervaltrigger-1)  | Constructs the trigger with type "IntervalTrigger" and maxTimes = 0 (unlimited). |
| `void` | [`update`](#update-3) `virtual` | Advances scheduleAt by one `interval` period. |
| `bool` | [`expired`](#expired-2) `virtual` | Returns true when maxTimes > 0 and timesRun >= maxTimes. |
| `void` | [`serialize`](#serialize-2) `virtual` | Serializes interval fields (days, hours, minutes, seconds) in addition to base fields.  |
| `void` | [`deserialize`](#deserialize-2) `virtual` | Deserializes interval fields from `root`. Throws if the resulting interval is zero.  |

---

{#intervaltrigger-1}

#### IntervalTrigger

```cpp
IntervalTrigger()
```

Constructs the trigger with type "IntervalTrigger" and maxTimes = 0 (unlimited).

---

{#update-3}

#### update

`virtual`

```cpp
virtual void update()
```

Advances scheduleAt by one `interval` period.

---

{#expired-2}

#### expired

`virtual`

```cpp
virtual bool expired()
```

Returns true when maxTimes > 0 and timesRun >= maxTimes.

---

{#serialize-2}

#### serialize

`virtual`

```cpp
virtual void serialize(json::Value & root)
```

Serializes interval fields (days, hours, minutes, seconds) in addition to base fields. 
#### Parameters
* `root` JSON object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

---

{#deserialize-2}

#### deserialize

`virtual`

```cpp
virtual void deserialize(json::Value & root)
```

Deserializes interval fields from `root`. Throws if the resulting interval is zero. 
#### Parameters
* `root` JSON object previously produced by [serialize()](#serialize-2).

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

{#onceonlytrigger}

## OnceOnlyTrigger

```cpp
#include <icy/sched/trigger.h>
```

> **Inherits:** [`Trigger`](#trigger-1)

[Trigger](#trigger-1) that fires exactly once at the scheduled time and then expires.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`OnceOnlyTrigger`](#onceonlytrigger-1)  | Constructs the trigger with type "OnceOnlyTrigger". |
| `void` | [`update`](#update-4) `virtual` `inline` | No-op; scheduleAt is set once at construction and never advanced. |
| `bool` | [`expired`](#expired-3) `virtual` | Returns true after the task has run at least once. |

---

{#onceonlytrigger-1}

#### OnceOnlyTrigger

```cpp
OnceOnlyTrigger()
```

Constructs the trigger with type "OnceOnlyTrigger".

---

{#update-4}

#### update

`virtual` `inline`

```cpp
virtual inline void update()
```

No-op; scheduleAt is set once at construction and never advanced.

---

{#expired-3}

#### expired

`virtual`

```cpp
virtual bool expired()
```

Returns true after the task has run at least once.

{#trigger-1}

## Trigger

```cpp
#include <icy/sched/trigger.h>
```

> **Inherits:** [`ISerializable`](json.md#iserializable)
> **Subclassed by:** [`DailyTrigger`](#dailytrigger), [`IntervalTrigger`](#intervaltrigger), [`OnceOnlyTrigger`](#onceonlytrigger)

Base class for scheduled task triggers that determine when a task should run.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`type`](#type-17)  | The type of this trigger class. |
| `std::string` | [`name`](#name-9)  | The display name of this trigger class. |
| `int` | [`timesRun`](#timesrun)  | The number of times the task has run since creation; |
| `DateTime` | [`createdAt`](#createdat)  | The time the task was created. |
| `DateTime` | [`scheduleAt`](#scheduleat)  | The time the task is scheduled to run. |
| `DateTime` | [`lastRunAt`](#lastrunat)  | The time the task was last run. |

---

{#type-17}

#### type

```cpp
std::string type
```

The type of this trigger class.

---

{#name-9}

#### name

```cpp
std::string name
```

The display name of this trigger class.

---

{#timesrun}

#### timesRun

```cpp
int timesRun
```

The number of times the task has run since creation;

---

{#createdat}

#### createdAt

```cpp
DateTime createdAt
```

The time the task was created.

---

{#scheduleat}

#### scheduleAt

```cpp
DateTime scheduleAt
```

The time the task is scheduled to run.

---

{#lastrunat}

#### lastRunAt

```cpp
DateTime lastRunAt
```

The time the task was last run.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Trigger`](#trigger-2)  | #### Parameters |
| `void` | [`update`](#update-6)  | Updates the scheduleAt value to the next scheduled time. |
| `std::int64_t` | [`remaining`](#remaining-1) `virtual` | Returns the milliseconds remaining until the next scheduled timeout. |
| `bool` | [`timeout`](#timeout-4) `virtual` | Returns true if the task is ready to be run, false otherwise. |
| `bool` | [`expired`](#expired-4) `virtual` | Returns true if the task is expired and should be destroyed. Returns false by default. |
| `void` | [`serialize`](#serialize-5) `virtual` | Serializes timing state (type, name, createdAt, scheduleAt, lastRunAt, timesRun) to `root`.  |
| `void` | [`deserialize`](#deserialize-5) `virtual` | Deserializes timing state from `root`.  |

---

{#trigger-2}

#### Trigger

```cpp
Trigger(const std::string & type, const std::string & name)
```

#### Parameters
* `type` Registered type name used by [TaskFactory](#taskfactory). 

* `name` Human-readable display name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `name` | `const std::string &` |  |

---

{#update-6}

#### update

```cpp
void update()
```

Updates the scheduleAt value to the next scheduled time.

---

{#remaining-1}

#### remaining

`virtual`

```cpp
virtual std::int64_t remaining()
```

Returns the milliseconds remaining until the next scheduled timeout.

---

{#timeout-4}

#### timeout

`virtual`

```cpp
virtual bool timeout()
```

Returns true if the task is ready to be run, false otherwise.

---

{#expired-4}

#### expired

`virtual`

```cpp
virtual bool expired()
```

Returns true if the task is expired and should be destroyed. Returns false by default.

---

{#serialize-5}

#### serialize

`virtual`

```cpp
virtual void serialize(json::Value & root)
```

Serializes timing state (type, name, createdAt, scheduleAt, lastRunAt, timesRun) to `root`. 
#### Parameters
* `root` JSON object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

---

{#deserialize-5}

#### deserialize

`virtual`

```cpp
virtual void deserialize(json::Value & root)
```

Deserializes timing state from `root`. 
#### Parameters
* `root` JSON object previously produced by [serialize()](#serialize-5).

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `[json::Value](json.md#value) &` |  |

