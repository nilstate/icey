///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#pragma once


#include "scy/datetime.h"
#include "scy/json/iserializable.h"
#include "scy/sched/sched.h"


namespace scy {
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
struct Trigger : public json::ISerializable
{
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

    virtual void serialize(json::Value& root) override;
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
struct OnceOnlyTrigger : public Trigger
{
    OnceOnlyTrigger();

    virtual void update() override
    {
        // Nothing to do since scheduleAt contains
        // the correct date and we run once only.
    }

    virtual bool expired() override;
};


// ---------------------------------------------------------------------
//
struct IntervalTrigger : public Trigger
{
    IntervalTrigger();

    virtual void update() override;
    virtual bool expired() override;

    virtual void serialize(json::Value& root) override;
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
struct DailyTrigger : public Trigger
{
    DailyTrigger();

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
} // namespace scy


/// @\}
