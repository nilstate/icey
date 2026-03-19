///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#include "icy/sched/trigger.h"
#include "icy/datetime.h"
#include "icy/logger.h"
#include "icy/sched/scheduler.h"


using namespace std;


namespace icy {
namespace sched {


Trigger::Trigger(const std::string& type, const std::string& name)
    : type(type)
    , name(name)
    , timesRun(0)
{
}


std::int64_t Trigger::remaining()
{
    DateTime now;
    Timespan ts = scheduleAt - now;
    return ts.totalMilliseconds();
}


bool Trigger::timeout()
{
    DateTime now;
    return now >= scheduleAt;
}


bool Trigger::expired()
{
    return false;
}


void Trigger::serialize(json::Value& root)
{
    LTrace("Serializing");

    root["type"] = type;
    root["name"] = name;
    root["createdAt"] =
        DateTimeFormatter::format(createdAt, DateTimeFormat::ISO8601_FORMAT);
    root["scheduleAt"] =
        DateTimeFormatter::format(scheduleAt, DateTimeFormat::ISO8601_FORMAT);
    root["lastRunAt"] =
        DateTimeFormatter::format(lastRunAt, DateTimeFormat::ISO8601_FORMAT);
    root["timesRun"] = timesRun;
}


void Trigger::deserialize(json::Value& root)
{
    LTrace("Deserializing");

    json::assertMember(root, "type");
    json::assertMember(root, "name");
    json::assertMember(root, "createdAt");
    json::assertMember(root, "scheduleAt");
    json::assertMember(root, "lastRunAt");
    json::assertMember(root, "timesRun");

    int tzd;
    type = root["type"].get<std::string>();
    name = root["name"].get<std::string>();
    createdAt = DateTimeParser::parse(DateTimeFormat::ISO8601_FORMAT,
                                      root["createdAt"].get<std::string>(), tzd);
    scheduleAt = DateTimeParser::parse(DateTimeFormat::ISO8601_FORMAT,
                                       root["scheduleAt"].get<std::string>(), tzd);
    lastRunAt = DateTimeParser::parse(DateTimeFormat::ISO8601_FORMAT,
                                      root["lastRunAt"].get<std::string>(), tzd);
    timesRun = root["timesRun"].get<int>();
}


// ---------------------------------------------------------------------
//
OnceOnlyTrigger::OnceOnlyTrigger()
    : Trigger("OnceOnlyTrigger", "Once Only")
{
}


bool OnceOnlyTrigger::expired()
{
    return timesRun > 0;
}


// ---------------------------------------------------------------------
//
IntervalTrigger::IntervalTrigger()
    : Trigger("IntervalTrigger", "Interval")
    , maxTimes(0)
{
}


void IntervalTrigger::update()
{
    scheduleAt += interval;
}


bool IntervalTrigger::expired()
{
    return maxTimes > 0 && timesRun >= maxTimes;
}


void IntervalTrigger::serialize(json::Value& root)
{
    LTrace("Serializing");

    Trigger::serialize(root);

    root["interval"]["days"] = interval.days();
    root["interval"]["hours"] = interval.hours();
    root["interval"]["minutes"] = interval.minutes();
    root["interval"]["seconds"] = interval.seconds();
}


void IntervalTrigger::deserialize(json::Value& root)
{
    LTrace("[IntervalTrigger] Deserializing");

    json::assertMember(root, "interval");
    json::assertMember(root["interval"], "days");
    json::assertMember(root["interval"], "hours");
    json::assertMember(root["interval"], "minutes");
    json::assertMember(root["interval"], "seconds");

    Trigger::deserialize(root);

    interval.assign(root["interval"]["days"].get<int>(),
                    root["interval"]["hours"].get<int>(),
                    root["interval"]["minutes"].get<int>(),
                    root["interval"]["seconds"].get<int>(), 0);

    if (!interval.totalSeconds())
        throw std::runtime_error(
            "Interval trigger must have non zero interval.");

    DateTime now;
    scheduleAt = now;
    scheduleAt += interval;
}


// ---------------------------------------------------------------------
//
DailyTrigger::DailyTrigger()
    : Trigger("DailyTrigger", "Daily")
{
}


void DailyTrigger::update()
{
    DateTime now;
    DateTime next;
    DateTime prev(scheduleAt);
    Timespan day(1, 0, 0, 0, 0);
    bool initial = createdAt == scheduleAt;

    // Set next date as tomorrow if the schedule
    // timeout is expired, or if we are setting up.
    if (!initial && now > scheduleAt)
        next += day;

    // Get the next day the task can run
    bool match = false;
    for (unsigned i = 0; i < 6; i++) {
        match = false;
        for (unsigned x = 0; x < daysExcluded.size(); x++) {
            if (daysExcluded[x] == next.dayOfWeek()) {
                match = true;

                // Increment the start date
                next += day;
            }
        }
        if (!match)
            break;
    }

    // Assign the next scheduled time
    scheduleAt.assign(next.year(), next.month(), next.day(), timeOfDay.hour(),
                      timeOfDay.minute(), timeOfDay.second(),
                      timeOfDay.millisecond(), timeOfDay.microsecond());

    /*
    STrace << "[DailyTrigger] Updating: "
            << "\n\tDayOfWeek: " << next.dayOfWeek()
            << "\n\tNowTime: " << DateTimeFormatter::format(now,
    Poco::DateTimeFormat::ISO8601_FORMAT)
            << "\n\tPrevTime: " << DateTimeFormatter::format(prev,
    Poco::DateTimeFormat::ISO8601_FORMAT)
            << "\n\tNextTime: " << DateTimeFormatter::format(next,
    Poco::DateTimeFormat::ISO8601_FORMAT)
            << "\n\tScheduleTime: " << DateTimeFormatter::format(scheduleAt,
    Poco::DateTimeFormat::ISO8601_FORMAT)
;
            */
}


} // namespace sched
} // namespace icy


/// @\}
