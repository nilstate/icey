///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup sched
/// @{


#include "icy/sched/trigger.h"
#include "icy/datetime.h"
#include "icy/logger.h"
#include "icy/sched/scheduler.h"

#include <algorithm>
#include <stdexcept>


using namespace std;


namespace icy {
namespace sched {


Trigger::Trigger(const std::string& type, const std::string& name)
    : type(type)
    , name(name)
    , timesRun(0)
{
}


void Trigger::prepareForSchedule()
{
}


void Trigger::restore()
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


void IntervalTrigger::prepareForSchedule()
{
    if (timesRun == 0 && scheduleAt == createdAt) {
        scheduleAt = createdAt;
        scheduleAt += interval;
    }
}


void IntervalTrigger::restore()
{
    if (!interval.totalMicroseconds())
        throw std::runtime_error(
            "Interval trigger must have non zero interval.");

    DateTime now;
    if (scheduleAt > now)
        return;

    auto intervalMicros = interval.totalMicroseconds();
    Timespan overdue = now - scheduleAt;
    auto steps = overdue.totalMicroseconds() / intervalMicros + 1;
    scheduleAt += Timespan(steps * intervalMicros);
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
    root["maxTimes"] = maxTimes;
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
    maxTimes = root.contains("maxTimes") ? root["maxTimes"].get<int>() : 0;

    interval.assign(root["interval"]["days"].get<int>(),
                    root["interval"]["hours"].get<int>(),
                    root["interval"]["minutes"].get<int>(),
                    root["interval"]["seconds"].get<int>(), 0);

    if (!interval.totalSeconds())
        throw std::runtime_error(
            "Interval trigger must have non zero interval.");
}


// ---------------------------------------------------------------------
//
DailyTrigger::DailyTrigger()
    : Trigger("DailyTrigger", "Daily")
{
}


namespace {


DateTime nextDailySchedule(const DateTime& now, const DateTime& timeOfDay,
                           const std::vector<DaysOfTheWeek>& daysExcluded,
                           bool allowToday)
{
    DateTime candidate(now.year(), now.month(), now.day(), timeOfDay.hour(),
                       timeOfDay.minute(), timeOfDay.second(),
                       timeOfDay.millisecond(), timeOfDay.microsecond());

    if (!allowToday || candidate <= now)
        candidate += Timespan(1, 0, 0, 0, 0);

    for (int i = 0; i < 7; ++i) {
        auto day = static_cast<DaysOfTheWeek>(candidate.dayOfWeek());
        if (std::find(daysExcluded.begin(), daysExcluded.end(), day) ==
            daysExcluded.end())
            return candidate;
        candidate += Timespan(1, 0, 0, 0, 0);
    }

    throw std::runtime_error(
        "Daily trigger excludes every day of the week.");
}


} // namespace


void DailyTrigger::update()
{
    DateTime now;
    scheduleAt = nextDailySchedule(now, timeOfDay, daysExcluded, false);
}


void DailyTrigger::prepareForSchedule()
{
    if (scheduleAt == createdAt) {
        DateTime now;
        scheduleAt = nextDailySchedule(now, timeOfDay, daysExcluded, true);
    }
}


void DailyTrigger::restore()
{
    DateTime now;
    scheduleAt = nextDailySchedule(now, timeOfDay, daysExcluded, true);
}


void DailyTrigger::serialize(json::Value& root)
{
    Trigger::serialize(root);
    root["timeOfDay"] =
        DateTimeFormatter::format(timeOfDay, DateTimeFormat::ISO8601_FORMAT);
    auto& excluded = root["daysExcluded"];
    for (const auto day : daysExcluded)
        excluded[excluded.size()] = day;
}


void DailyTrigger::deserialize(json::Value& root)
{
    Trigger::deserialize(root);

    json::assertMember(root, "timeOfDay");
    int tzd;
    timeOfDay = DateTimeParser::parse(DateTimeFormat::ISO8601_FORMAT,
                                      root["timeOfDay"].get<std::string>(), tzd);

    daysExcluded.clear();
    if (root.contains("daysExcluded")) {
        for (const auto& day : root["daysExcluded"]) {
            auto value = day.get<int>();
            if (value < Sunday || value > Saturday)
                throw std::runtime_error("Invalid daily trigger excluded day.");
            daysExcluded.push_back(static_cast<DaysOfTheWeek>(value));
        }
    }
}


} // namespace sched
} // namespace icy


/// @}
