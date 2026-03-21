# sched 

### Classes

| Name | Description |
|------|-------------|
| [`Scheduler`](#classicy_1_1sched_1_1Scheduler) | The [Scheduler](#classicy_1_1sched_1_1Scheduler) manages and runs tasks that need to be executed at specific times. |
| [`Task`](#classicy_1_1sched_1_1Task) | This class extends the [Task](#classicy_1_1sched_1_1Task) class to implement scheduling capabilities. |
| [`TaskFactory`](#classicy_1_1sched_1_1TaskFactory) | The [TaskFactory](#classicy_1_1sched_1_1TaskFactory) can dynamically instantiate registered [sched::Task](#classicy_1_1sched_1_1Task) and [sched::Trigger](#structicy_1_1sched_1_1Trigger) classes from named strings. |
| [`Trigger`](#structicy_1_1sched_1_1Trigger) |  |
| [`OnceOnlyTrigger`](#structicy_1_1sched_1_1OnceOnlyTrigger) |  |
| [`IntervalTrigger`](#structicy_1_1sched_1_1IntervalTrigger) |  |
| [`DailyTrigger`](#structicy_1_1sched_1_1DailyTrigger) |  |

### Members

| Name | Description |
|------|-------------|
| [`Sched_API`](#group__sched_1ga4441e9df8956fd926aec04e6f1492ac9) |  |

---

#### Sched_API 

```cpp
Sched_API()
```

## Scheduler 

> **Extends:** `icy::TaskRunner`, `icy::json::ISerializable`
> **Defined in:** `scheduler.h`

The [Scheduler](#classicy_1_1sched_1_1Scheduler) manages and runs tasks that need to be executed at specific times.

### Members

| Name | Description |
|------|-------------|
| [`Scheduler`](#group__sched_1ga2c5d8bd2230a1f7bf4f073f28110d936) |  |
| [`~Scheduler`](#group__sched_1ga4a1c46d3daf229f48641fe62d73b3d8f) |  |
| [`schedule`](#group__sched_1gab33387aa6f9c21d61dbd9ed75da33112) |  |
| [`cancel`](#group__sched_1ga6dd66192cc1d853e8aea46206f9deeb0) |  |
| [`clear`](#group__sched_1gad1100d94ca6a33a5cd7e12ec4b414ebb) | Destroys and clears all manages tasks. |
| [`serialize`](#group__sched_1ga4c35dfb97b425dfe52b7d0423b6e1c32) |  |
| [`deserialize`](#group__sched_1ga7fbf9c5bdc302b447f5dfecb4d1e4f01) |  |
| [`print`](#group__sched_1gace1e075e5d80427459eb6a9f431d82c7) |  |
| [`getDefault`](#group__sched_1ga69f78c44f8ca4f6c98de61beb8f509d3) | Returns the default [Scheduler](#classicy_1_1sched_1_1Scheduler) singleton, although [Scheduler](#classicy_1_1sched_1_1Scheduler) instances may also be initialized individually. |
| [`factory`](#group__sched_1gaebe718c34d043b8995e5cde73d637f6b) | Returns the [TaskFactory](#classicy_1_1sched_1_1TaskFactory) singleton. |
| [`run`](#group__sched_1gae90f548144a297df629384b7c19753af) | Called by the async context to run the next task. |
| [`update`](#group__sched_1ga59ae89e12b6fc6cc7d4d3acf9aeb5138) |  |

---

#### Scheduler 

```cpp
Scheduler()
```

---

#### ~Scheduler 

```cpp
virtual ~Scheduler()
```

---

#### schedule 

```cpp
virtual void schedule(sched::Task * task)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `sched::Task *` |  |

---

#### cancel 

```cpp
virtual void cancel(sched::Task * task)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `sched::Task *` |  |

---

#### clear 

```cpp
virtual void clear()
```

Destroys and clears all manages tasks.

---

#### serialize 

```cpp
virtual void serialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### deserialize 

```cpp
virtual void deserialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### getDefault 

```cpp
static Scheduler & getDefault()
```

Returns the default [Scheduler](#classicy_1_1sched_1_1Scheduler) singleton, although [Scheduler](#classicy_1_1sched_1_1Scheduler) instances may also be initialized individually.

---

#### factory 

```cpp
static sched::TaskFactory & factory()
```

Returns the [TaskFactory](#classicy_1_1sched_1_1TaskFactory) singleton.

---

#### run 

```cpp
virtual void run()
```

Called by the async context to run the next task.

---

#### update 

```cpp
virtual void update()
```

## Task 

> **Extends:** `icy::Task`, `icy::json::ISerializable`
> **Defined in:** `task.h`

This class extends the [Task](#classicy_1_1sched_1_1Task) class to implement scheduling capabilities.

### Members

| Name | Description |
|------|-------------|
| [`Task`](#group__sched_1gada7aa3c08477dd31f77a15d9ab7ada69) |  |
| [`Task`](#group__sched_1ga18514602a261599745e9d632a9298639) |  |
| [`serialize`](#group__sched_1gad18e09c30fab7855d8c3c54f5e1430bb) | Serializes the task to JSON. |
| [`deserialize`](#group__sched_1gaf213bcf307f7831a73a53e20f2973ff0) | Deserializes the task from JSON. |
| [`createTrigger`](#group__sched_1ga5ccc658ea51817c922302024f1716d0c) |  |
| [`setTrigger`](#group__sched_1ga750447b4c974d32fa66e6bf382968364) |  |
| [`trigger`](#group__sched_1gaeca4e399907b97a311542cf3f6c02ac5) | Returns a reference to the associated [sched::Trigger](#structicy_1_1sched_1_1Trigger) or throws an exception. |
| [`scheduler`](#group__sched_1ga194bdc6aa8624503a044fe24e1b1ed34) | Returns a reference to the associated [Scheduler](#classicy_1_1sched_1_1Scheduler) or throws an exception. |
| [`remaining`](#group__sched_1ga5c18d3c47370a09baaff18b12ecb6779) | Returns the milliseconds remaining until the next scheduled timeout. An [sched::Trigger](#structicy_1_1sched_1_1Trigger) must be associated or an exception will be thrown. |
| [`type`](#group__sched_1gab2487a3713a57261b71454c05621dbcd) |  |
| [`name`](#group__sched_1gadfe739e111da7d1d090c88de52da1767) |  |
| [`setName`](#group__sched_1gaf82f66d275ae0a0b750c3ef3b8785093) |  |
| [`~Task`](#group__sched_1ga92fdc84e904656f4e38cdc7acd8d6053) | Destroctor. Should remain protected. |
| [`beforeRun`](#group__sched_1gabcf93ea5a1c4c307524ce3711434ee39) |  |
| [`run`](#group__sched_1ga3d2d7a26a59ef58967926367d6ba484a) | Called by the [TaskRunner](./doc/api-base.md#classicy_1_1TaskRunner) to run the task. Override this method to implement task action. Returning true means the true should be called again, and false will cause the task to be destroyed. The task will similarly be destroyed id [destroy()](./doc/api-base.md#group__base_1gaecd8a85018f8f75ae802dcbf3749bdef) was called during the current task iteration. |
| [`afterRun`](#group__sched_1gae7f7a8c0ead6727644d3aa045b0c25df) |  |
| [`CompareTimeout`](#group__sched_1gaa91e1e6bcae34a4975f8ab33dd050373) |  |
| [`_type`](#group__sched_1gac28fe32991a883c1b00a6b56bc555c35) |  |
| [`_name`](#group__sched_1ga5c053d1a5d387b942e8cb55a9bfc2fac) |  |
| [`_scheduler`](#group__sched_1ga1fe5675a303013606d55d54b79700557) |  |
| [`_trigger`](#group__sched_1ga6627c1994358922d5dc892df16d7ab6e) |  |
| [`_mutex`](#group__sched_1ga1233679a60cc60f163e2e63988701747) |  |

---

#### Task 

```cpp
Task(const std::string & type, const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `name` | `const std::string &` |  |

---

#### Task 

```cpp
Task(Scheduler & scheduler, const std::string & type, const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheduler` | `Scheduler &` |  |
| `type` | `const std::string &` |  |
| `name` | `const std::string &` |  |

---

#### serialize 

```cpp
virtual void serialize(json::value & root)
```

Serializes the task to JSON.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### deserialize 

```cpp
virtual void deserialize(json::value & root)
```

Deserializes the task from JSON.

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### createTrigger 

```cpp
template<typename T> inline T * createTrigger()
```

---

#### setTrigger 

```cpp
void setTrigger(sched::Trigger * trigger)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `trigger` | `sched::Trigger *` |  |

---

#### trigger 

```cpp
sched::Trigger & trigger()
```

Returns a reference to the associated [sched::Trigger](#structicy_1_1sched_1_1Trigger) or throws an exception.

---

#### scheduler 

```cpp
Scheduler & scheduler()
```

Returns a reference to the associated [Scheduler](#classicy_1_1sched_1_1Scheduler) or throws an exception.

---

#### remaining 

```cpp
std::int64_t remaining() const
```

Returns the milliseconds remaining until the next scheduled timeout. An [sched::Trigger](#structicy_1_1sched_1_1Trigger) must be associated or an exception will be thrown.

---

#### type 

```cpp
std::string type() const
```

---

#### name 

```cpp
std::string name() const
```

---

#### setName 

```cpp
void setName(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### ~Task 

```cpp
virtual ~Task()
```

Destroctor. Should remain protected.

---

#### beforeRun 

```cpp
virtual bool beforeRun()
```

---

#### run 

```cpp
void run()
```

Called by the [TaskRunner](./doc/api-base.md#classicy_1_1TaskRunner) to run the task. Override this method to implement task action. Returning true means the true should be called again, and false will cause the task to be destroyed. The task will similarly be destroyed id [destroy()](./doc/api-base.md#group__base_1gaecd8a85018f8f75ae802dcbf3749bdef) was called during the current task iteration.

---

#### afterRun 

```cpp
virtual bool afterRun()
```

---

#### CompareTimeout 

```cpp
static inline bool CompareTimeout(const [icy::Task](./doc/api-base.md#classicy_1_1Task) * l, const [icy::Task](./doc/api-base.md#classicy_1_1Task) * r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `l` | `const [icy::Task](./doc/api-base.md#classicy_1_1Task) *` |  |
| `r` | `const [icy::Task](./doc/api-base.md#classicy_1_1Task) *` |  |

---

#### _type 

```cpp
std::string _type
```

---

#### _name 

```cpp
std::string _name
```

---

#### _scheduler 

```cpp
sched::Scheduler * _scheduler
```

---

#### _trigger 

```cpp
sched::Trigger * _trigger
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## TaskFactory 

> **Defined in:** `taskfactory.h`

The [TaskFactory](#classicy_1_1sched_1_1TaskFactory) can dynamically instantiate registered [sched::Task](#classicy_1_1sched_1_1Task) and [sched::Trigger](#structicy_1_1sched_1_1Trigger) classes from named strings.

### Members

| Name | Description |
|------|-------------|
| [`getDefault`](#group__sched_1ga09f76779885d97c4e99bf27c349909d1) | Returns the default [TaskFactory](#classicy_1_1sched_1_1TaskFactory) singleton. |
| [`createTask`](#group__sched_1gafa680c1c63b36eaa94986f3548ec301b) |  |
| [`registerTask`](#group__sched_1ga3b265588e84942014d4d8ed689d71b2d) |  |
| [`unregisterTask`](#group__sched_1ga16d6fa9512bcf4e4b3deb0f1f6807286) |  |
| [`tasks`](#group__sched_1ga41531034b6b18d3a04b04279fe69f587) |  |
| [`createTrigger`](#group__sched_1ga71080ec4924c904a67c07cb872654e99) |  |
| [`registerTrigger`](#group__sched_1ga1cd0703715177bfc010ac24975120e9b) |  |
| [`unregisterTrigger`](#group__sched_1gac05f05edd58089bd2fd37031dd3a2785) |  |
| [`triggers`](#group__sched_1ga4c9f42c5dd1ae286d5d4c1e515740e50) |  |
| [`_mutex`](#group__sched_1gad8e95336c38e4f191d7f27975a8543a4) |  |
| [`_tasks`](#group__sched_1ga852d826a6712845f19aafa5d5588c4fc) |  |
| [`_triggers`](#group__sched_1gaa63f76a92ad76b48cea935bda6ac7535) |  |

---

#### getDefault 

```cpp
static inline TaskFactory & getDefault()
```

Returns the default [TaskFactory](#classicy_1_1sched_1_1TaskFactory) singleton.

---

#### createTask 

```cpp
inline sched::Task * createTask(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### registerTask 

```cpp
template<typename T> inline void registerTask(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### unregisterTask 

```cpp
inline void unregisterTask(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### tasks 

```cpp
inline TaskMap tasks() const
```

---

#### createTrigger 

```cpp
inline sched::Trigger * createTrigger(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### registerTrigger 

```cpp
template<typename T> inline void registerTrigger(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### unregisterTrigger 

```cpp
inline void unregisterTrigger(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### triggers 

```cpp
inline TriggerMap triggers() const
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _tasks 

```cpp
TaskMap _tasks
```

---

#### _triggers 

```cpp
TriggerMap _triggers
```

## Trigger 

> **Extends:** `icy::json::ISerializable`
> **Subclasses:** `icy::sched::DailyTrigger`, `icy::sched::IntervalTrigger`, `icy::sched::OnceOnlyTrigger`
> **Defined in:** `trigger.h`

### Members

| Name | Description |
|------|-------------|
| [`Trigger`](#group__sched_1ga1d108947ea17eb1787d2bbd9305dda59) |  |
| [`update`](#group__sched_1ga5deb765be5af1396d7cdfb2904c426d8) | Updates the scheduleAt value to the next scheduled time. |
| [`remaining`](#group__sched_1gaaab35f447a018bbda0742b5bd9992f71) | Returns the milliseconds remaining until the next scheduled timeout. |
| [`timeout`](#group__sched_1gaa20be8a2498f10c86bdb7bf87b39cb70) | Returns true if the task is ready to be run, false otherwise. |
| [`expired`](#group__sched_1ga6085d6dc573d7034cbf55f70749f6e73) | Returns true if the task is expired and should be destroyed. Returns false by default. |
| [`serialize`](#group__sched_1ga78f847987a4e72cd79e5ec95d999725f) |  |
| [`deserialize`](#group__sched_1ga756cab4a32e06179aa19a6eff84a473a) |  |
| [`type`](#group__sched_1ga80dbc210eee2539299264057d881959a) | The type of this trigger class. |
| [`name`](#group__sched_1ga3e24e172d92dc417863569d8b95323d9) | The display name of this trigger class. |
| [`timesRun`](#group__sched_1ga16517ce07a40cd2e66fd8065fa998b7b) | The number of times the task has run since creation; |
| [`createdAt`](#group__sched_1ga36015f8af492436ca74de13d7a43ce79) | The time the task was created. |
| [`scheduleAt`](#group__sched_1ga6435f36f242fb144219d7b05600a4e1c) | The time the task is scheduled to run. |
| [`lastRunAt`](#group__sched_1ga9ed36dda7c8943d3644e295774a9a213) | The time the task was last run. |

---

#### Trigger 

```cpp
Trigger(const std::string & type, const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `name` | `const std::string &` |  |

---

#### update 

```cpp
void update()
```

Updates the scheduleAt value to the next scheduled time.

---

#### remaining 

```cpp
virtual std::int64_t remaining()
```

Returns the milliseconds remaining until the next scheduled timeout.

---

#### timeout 

```cpp
virtual bool timeout()
```

Returns true if the task is ready to be run, false otherwise.

---

#### expired 

```cpp
virtual bool expired()
```

Returns true if the task is expired and should be destroyed. Returns false by default.

---

#### serialize 

```cpp
virtual void serialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### deserialize 

```cpp
virtual void deserialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### type 

```cpp
std::string type
```

The type of this trigger class.

---

#### name 

```cpp
std::string name
```

The display name of this trigger class.

---

#### timesRun 

```cpp
int timesRun
```

The number of times the task has run since creation;

---

#### createdAt 

```cpp
DateTime createdAt
```

The time the task was created.

---

#### scheduleAt 

```cpp
DateTime scheduleAt
```

The time the task is scheduled to run.

---

#### lastRunAt 

```cpp
DateTime lastRunAt
```

The time the task was last run.

## OnceOnlyTrigger 

> **Extends:** `icy::sched::Trigger`
> **Defined in:** `trigger.h`

### Members

| Name | Description |
|------|-------------|
| [`OnceOnlyTrigger`](#group__sched_1ga134b95cccf5189e5e707b55437835436) |  |
| [`update`](#group__sched_1ga0a1482f3273f7ac2ad93cd587d470d8a) | Updates the scheduleAt value to the next scheduled time. |
| [`expired`](#group__sched_1ga6c7f09ec6a53ba5603e1f8694fef7b1e) | Returns true if the task is expired and should be destroyed. Returns false by default. |

---

#### OnceOnlyTrigger 

```cpp
OnceOnlyTrigger()
```

---

#### update 

```cpp
virtual inline void update()
```

Updates the scheduleAt value to the next scheduled time.

---

#### expired 

```cpp
virtual bool expired()
```

Returns true if the task is expired and should be destroyed. Returns false by default.

## IntervalTrigger 

> **Extends:** `icy::sched::Trigger`
> **Defined in:** `trigger.h`

### Members

| Name | Description |
|------|-------------|
| [`IntervalTrigger`](#group__sched_1gad32ce801f384e0a4a4a4e19acfe3a34c) |  |
| [`update`](#group__sched_1gaa0b8bbfe4eae4fe43dab985d5c248f76) | Updates the scheduleAt value to the next scheduled time. |
| [`expired`](#group__sched_1ga203c0a6cc5ccc67d29912187a8d5b6be) | Returns true if the task is expired and should be destroyed. Returns false by default. |
| [`serialize`](#group__sched_1ga902a0eb64586b66d47fa77d41a2a3925) |  |
| [`deserialize`](#group__sched_1ga3ff1dce27e772257407c6dfbf834a6cf) |  |
| [`interval`](#group__sched_1ga79c8a845365cd31f534b730e022e8d4d) | This value represents the interval to wait before running the task. |
| [`maxTimes`](#group__sched_1ga80f340cb8d0f23d4b1a89b2e200518b8) | The maximum number of times the task will be run before it is destroyed. 0 for no effect. |

---

#### IntervalTrigger 

```cpp
IntervalTrigger()
```

---

#### update 

```cpp
virtual void update()
```

Updates the scheduleAt value to the next scheduled time.

---

#### expired 

```cpp
virtual bool expired()
```

Returns true if the task is expired and should be destroyed. Returns false by default.

---

#### serialize 

```cpp
virtual void serialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### deserialize 

```cpp
virtual void deserialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### interval 

```cpp
Timespan interval
```

This value represents the interval to wait before running the task.

---

#### maxTimes 

```cpp
int maxTimes
```

The maximum number of times the task will be run before it is destroyed. 0 for no effect.

## DailyTrigger 

> **Extends:** `icy::sched::Trigger`
> **Defined in:** `trigger.h`

### Members

| Name | Description |
|------|-------------|
| [`DailyTrigger`](#group__sched_1ga5bbeff00daa5b34e548ec34276c599b0) |  |
| [`update`](#group__sched_1ga3cf631dce5a9d307f9162e8d5b6ed4f8) | Updates the scheduleAt value to the next scheduled time. |
| [`timeOfDay`](#group__sched_1gac27c59a1258d294183119fd52ac5b4fc) | This value represents the time of day the task will trigger. The date part of the timestamp is redundant. |
| [`daysExcluded`](#group__sched_1ga336f20188705b1fa837686bb01898409) | Days of the week can be excluded by adding the appropriate bit flag here. |

---

#### DailyTrigger 

```cpp
DailyTrigger()
```

---

#### update 

```cpp
virtual void update()
```

Updates the scheduleAt value to the next scheduled time.

---

#### timeOfDay 

```cpp
DateTime timeOfDay
```

This value represents the time of day the task will trigger. The date part of the timestamp is redundant.

---

#### daysExcluded 

```cpp
std::vector< DaysOfTheWeek > daysExcluded
```

Days of the week can be excluded by adding the appropriate bit flag here.

