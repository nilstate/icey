///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#pragma once


#include "icy/datetime.h"
#include "icy/json/iserializable.h"
#include "icy/sched/sched.h"


namespace icy {
namespace sched {


/// Days of the week
enum DaysOfTheWeek
{
    Sunday = 0,
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6
};


/// Months of the year
enum MonthOfTheYeay
{
    January = 0,
    February = 1,
    March = 2,
    April = 3,
    May = 4,
    June = 5,
    July = 6,
    August = 7,
    September = 8,
    October = 9,
    November = 10,
    December = 11
};


// ---------------------------------------------------------------------
//
/// Base class for scheduled task triggers that determine when a task should run
struct Trigger : public json::ISerializable
{
    /// @param type Registered type name used by TaskFactory.
    /// @param name Human-readable display name.
    Trigger(const std::string& type = "", const std::string& name = "");

    /// Updates the scheduleAt value to the
    /// next scheduled time.
    virtual void update() = 0;

    /// Returns the milliseconds remaining
    /// until the next scheduled timeout.
    virtual std::int64_t remaining();

    /// Returns true if the task is ready to
    /// be run, false otherwise.
    virtual bool timeout();

    /// Returns true if the task is expired
    /// and should be destroyed.
    /// Returns false by default.
    virtual bool expired();

    /// Serializes timing state (type, name, createdAt, scheduleAt, lastRunAt, timesRun) to @p root.
    /// @param root JSON object to populate.
    virtual void serialize(json::Value& root) override;

    /// Deserializes timing state from @p root.
    /// @param root JSON object previously produced by serialize().
    virtual void deserialize(json::Value& root) override;

    /// The type of this trigger class.
    std::string type;

    /// The display name of this trigger class.
    std::string name;

    /// The number of times the task has run
    /// since creation;
    int timesRun;

    /// The time the task was created.
    DateTime createdAt;

    /// The time the task is scheduled to run.
    DateTime scheduleAt;

    /// The time the task was last run.
    DateTime lastRunAt;
};


// ---------------------------------------------------------------------
//
/// Trigger that fires exactly once at the scheduled time and then expires
struct OnceOnlyTrigger : public Trigger
{
    /// Constructs the trigger with type "OnceOnlyTrigger".
    OnceOnlyTrigger();

    /// No-op; scheduleAt is set once at construction and never advanced.
    virtual void update() override
    {
        // Nothing to do since scheduleAt contains
        // the correct date and we run once only.
    }

    /// Returns true after the task has run at least once.
    virtual bool expired();
};


// ---------------------------------------------------------------------
//
/// Trigger that fires repeatedly at a fixed time interval
struct IntervalTrigger : public Trigger
{
    /// Constructs the trigger with type "IntervalTrigger" and maxTimes = 0 (unlimited).
    IntervalTrigger();

    /// Advances scheduleAt by one `interval` period.
    virtual void update() override;

    /// Returns true when maxTimes > 0 and timesRun >= maxTimes.
    virtual bool expired();

    /// Serializes interval fields (days, hours, minutes, seconds) in addition to base fields.
    /// @param root JSON object to populate.
    virtual void serialize(json::Value& root) override;

    /// Deserializes interval fields from @p root.
    /// Throws if the resulting interval is zero.
    /// @param root JSON object previously produced by serialize().
    virtual void deserialize(json::Value& root) override;

    /// This value represents the interval to wait
    /// before running the task.
    Timespan interval;

    /// The maximum number of times the task will
    /// be run before it is destroyed.
    /// 0 for no effect.
    int maxTimes;
};


// ---------------------------------------------------------------------
//
/// Trigger that fires once per day at a configured time, with optional day-of-week exclusions
struct DailyTrigger : public Trigger
{
    /// Constructs the trigger with type "DailyTrigger".
    DailyTrigger();

    /// Computes the next scheduleAt value by advancing to the next non-excluded weekday
    /// at the configured timeOfDay.
    virtual void update() override;

    /// This value represents the time of day the
    /// task will trigger.
    /// The date part of the timestamp is redundant.
    DateTime timeOfDay;

    /// Days of the week can be excluded by adding
    /// the appropriate bit flag here.
    std::vector<DaysOfTheWeek> daysExcluded;
};


} // namespace sched
} // namespace icy


/// @}
