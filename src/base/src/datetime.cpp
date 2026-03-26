///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/datetime.h"
#include "icy/numeric.h"
#include "icy/platform.h"
#include "icy/time.h"
#include "icy/util.h"

#include <cctype>
#include <chrono>
#include <cmath>
#include <ctime>




namespace icy {


// Number of 100ns intervals between the Gregorian epoch (Oct 15, 1582)
// and the Unix epoch (Jan 1, 1970).
static constexpr int64_t GREG_EPOCH_OFFSET =
    (int64_t(0x01b21dd2) << 32) + 0x13814000;


//
// Date Time
//


inline double DateTime::toJulianDay(Timestamp::UtcTimeVal utcTime)
{
    double utcDays = double(utcTime) / 864000000000.0;
    return utcDays + 2299160.5; // first day of Gregorian reform (Oct 15 1582)
}


inline Timestamp::UtcTimeVal DateTime::toUtcTime(double julianDay)
{
    return Timestamp::UtcTimeVal((julianDay - 2299160.5) * 864000000000.0);
}


DateTime::DateTime()
{
    Timestamp now;
    _utcTime = now.utcTime();
    computeGregorian(julianDay());
    computeDaytime();
}


DateTime::DateTime(const Timestamp& timestamp)
    : _utcTime(timestamp.utcTime())
{
    computeGregorian(julianDay());
    computeDaytime();
}


DateTime::DateTime(int year, int month, int day, int hour, int minute,
                   int second, int millisecond, int microsecond)
    : _year(static_cast<short>(year))
    , _month(static_cast<short>(month))
    , _day(static_cast<short>(day))
    , _hour(static_cast<short>(hour))
    , _minute(static_cast<short>(minute))
    , _second(static_cast<short>(second))
    , _millisecond(static_cast<short>(millisecond))
    , _microsecond(static_cast<short>(microsecond))
{
    if (year < 0 || year > 9999 || month < 1 || month > 12 ||
        day < 1 || day > daysOfMonth(year, month) ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59 || millisecond < 0 || millisecond > 999 ||
        microsecond < 0 || microsecond > 999)
        throw std::invalid_argument("DateTime: invalid date/time component");

    using namespace std::chrono;
    auto dp = sys_days{std::chrono::year{year} /
                       std::chrono::month{static_cast<unsigned>(month)} /
                       std::chrono::day{static_cast<unsigned>(day)}};
    auto us = duration_cast<microseconds>(dp.time_since_epoch())
        + hours{hour} + minutes{minute} + seconds{second}
        + milliseconds{millisecond} + microseconds{microsecond};
    _utcTime = us.count() * 10 + GREG_EPOCH_OFFSET;
}


DateTime::DateTime(double julianDay)
    : _utcTime(toUtcTime(julianDay))
{
    computeGregorian(julianDay);
    computeDaytime();
}


DateTime::DateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff)
    : _utcTime(utcTime + diff * 10)
{
    computeGregorian(julianDay());
    computeDaytime();
}


DateTime::DateTime(const DateTime& dateTime)
    : _utcTime(dateTime._utcTime)
    , _year(dateTime._year)
    , _month(dateTime._month)
    , _day(dateTime._day)
    , _hour(dateTime._hour)
    , _minute(dateTime._minute)
    , _second(dateTime._second)
    , _millisecond(dateTime._millisecond)
    , _microsecond(dateTime._microsecond)
{
}


DateTime::~DateTime()
{
}


DateTime& DateTime::operator=(const DateTime& dateTime)
{
    if (&dateTime != this) {
        _utcTime = dateTime._utcTime;
        _year = dateTime._year;
        _month = dateTime._month;
        _day = dateTime._day;
        _hour = dateTime._hour;
        _minute = dateTime._minute;
        _second = dateTime._second;
        _millisecond = dateTime._millisecond;
        _microsecond = dateTime._microsecond;
    }
    return *this;
}


DateTime& DateTime::operator=(const Timestamp& timestamp)
{
    _utcTime = timestamp.utcTime();
    computeGregorian(julianDay());
    computeDaytime();
    return *this;
}


DateTime& DateTime::operator=(double julianDay)
{
    _utcTime = toUtcTime(julianDay);
    computeGregorian(julianDay);
    computeDaytime();
    return *this;
}


DateTime& DateTime::assign(int year, int month, int day, int hour, int minute,
                           int second, int millisecond, int microsecond)
{
    if (year < 0 || year > 9999 || month < 1 || month > 12 ||
        day < 1 || day > daysOfMonth(year, month) ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59 || millisecond < 0 || millisecond > 999 ||
        microsecond < 0 || microsecond > 999)
        throw std::invalid_argument("DateTime::assign: invalid date/time component");

    using namespace std::chrono;
    auto dp = sys_days{std::chrono::year{year} /
                       std::chrono::month{static_cast<unsigned>(month)} /
                       std::chrono::day{static_cast<unsigned>(day)}};
    auto us = duration_cast<microseconds>(dp.time_since_epoch())
        + hours{hour} + minutes{minute} + seconds{second}
        + milliseconds{millisecond} + microseconds{microsecond};
    _utcTime = us.count() * 10 + GREG_EPOCH_OFFSET;
    _year = static_cast<short>(year);
    _month = static_cast<short>(month);
    _day = static_cast<short>(day);
    _hour = static_cast<short>(hour);
    _minute = static_cast<short>(minute);
    _second = static_cast<short>(second);
    _millisecond = static_cast<short>(millisecond);
    _microsecond = static_cast<short>(microsecond);

    return *this;
}


void DateTime::swap(DateTime& dateTime)
{
    std::swap(_utcTime, dateTime._utcTime);
    std::swap(_year, dateTime._year);
    std::swap(_month, dateTime._month);
    std::swap(_day, dateTime._day);
    std::swap(_hour, dateTime._hour);
    std::swap(_minute, dateTime._minute);
    std::swap(_second, dateTime._second);
    std::swap(_millisecond, dateTime._millisecond);
    std::swap(_microsecond, dateTime._microsecond);
}


int DateTime::dayOfWeek() const
{
    using namespace std::chrono;
    weekday wd{sys_days{std::chrono::year{_year} /
                        std::chrono::month{static_cast<unsigned>(_month)} /
                        std::chrono::day{static_cast<unsigned>(_day)}}};
    return static_cast<int>(wd.c_encoding()); // 0=Sunday
}


int DateTime::dayOfYear() const
{
    int doy = 0;
    for (int month = 1; month < _month; ++month)
        doy += daysOfMonth(_year, month);
    doy += _day;
    return doy;
}


int DateTime::daysOfMonth(int year, int month)
{
    if (month < 1 || month > 12)
        throw std::invalid_argument("DateTime::daysOfMonth: month out of range");

    static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30,
                                     31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year))
        return 29;
    else
        return daysOfMonthTable[month];
}


bool DateTime::isValid(int year, int month, int day, int hour, int minute,
                       int second, int millisecond, int microsecond)
{
    return (year >= 0 && year <= 9999) && (month >= 1 && month <= 12) &&
           (day >= 1 && day <= daysOfMonth(year, month)) &&
           (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59) &&
           (second >= 0 && second <= 59) &&
           (millisecond >= 0 && millisecond <= 999) &&
           (microsecond >= 0 && microsecond <= 999);
}


int DateTime::week(int firstDayOfWeek) const
{
    if (firstDayOfWeek < 0 || firstDayOfWeek > 6)
        throw std::invalid_argument("DateTime::week: firstDayOfWeek out of range");

    // find the first firstDayOfWeek.
    int baseDay = 1;
    while (DateTime(_year, 1, baseDay).dayOfWeek() != firstDayOfWeek)
        ++baseDay;

    int doy = dayOfYear();
    int offs = baseDay <= 4 ? 0 : 1;
    if (doy < baseDay)
        return offs;
    else
        return (doy - baseDay) / 7 + 1 + offs;
}


double DateTime::julianDay() const
{
    return toJulianDay(_utcTime);
}


DateTime DateTime::operator+(const Timespan& span) const
{
    return DateTime(_utcTime, span.totalMicroseconds());
}


DateTime DateTime::operator-(const Timespan& span) const
{
    return DateTime(_utcTime, -span.totalMicroseconds());
}


Timespan DateTime::operator-(const DateTime& dateTime) const
{
    return Timespan((_utcTime - dateTime._utcTime) / 10);
}


DateTime& DateTime::operator+=(const Timespan& span)
{
    _utcTime += span.totalMicroseconds() * 10;
    computeGregorian(julianDay());
    computeDaytime();
    return *this;
}


DateTime& DateTime::operator-=(const Timespan& span)
{
    _utcTime -= span.totalMicroseconds() * 10;
    computeGregorian(julianDay());
    computeDaytime();
    return *this;
}


void DateTime::makeUTC(int tzd)
{
    operator-=(Timespan(((Timestamp::TimeDiff)tzd) * Timespan::SECONDS));
}


void DateTime::makeLocal(int tzd)
{
    operator+=(Timespan(((Timestamp::TimeDiff)tzd) * Timespan::SECONDS));
}


double DateTime::toJulianDay(int year, int month, int day, int hour, int minute,
                             int second, int millisecond, int microsecond)
{
    // lookup table for (153*month - 457)/5 - note that 3 <= month <= 14.
    static int lookup[] = {-91, -60, -30, 0, 31, 61, 92, 122,
                           153, 184, 214, 245, 275, 306, 337};

    // day to double
    double dday = double(day) +
                  ((double((hour * 60 + minute) * 60 + second) * 1000 + millisecond) *
                       1000 +
                   microsecond) /
                      86400000000.0;
    if (month < 3) {
        month += 12;
        --year;
    }
    double dyear = double(year);
    return dday + lookup[month] + 365 * year + std::floor(dyear / 4) -
           std::floor(dyear / 100) + std::floor(dyear / 400) + 1721118.5;
}


void DateTime::checkLimit(short& lower, short& higher, short limit)
{
    if (lower >= limit) {
        higher += short(lower / limit);
        lower = short(lower % limit);
    }
}


void DateTime::normalize()
{
    checkLimit(_microsecond, _millisecond, 1000);
    checkLimit(_millisecond, _second, 1000);
    checkLimit(_second, _minute, 60);
    checkLimit(_minute, _hour, 60);
    checkLimit(_hour, _day, 24);

    if (_day > daysOfMonth(_year, _month)) {
        _day -= static_cast<short>(daysOfMonth(_year, _month));
        if (++_month > 12) {
            ++_year;
            _month -= 12;
        }
    }
}


void DateTime::computeGregorian(double)
{
    using namespace std::chrono;
    auto us = microseconds((_utcTime - GREG_EPOCH_OFFSET) / 10);
    auto tp = system_clock::time_point(duration_cast<system_clock::duration>(us));
    auto dp = floor<days>(tp);
    year_month_day ymd{dp};
    _year = static_cast<short>(static_cast<int>(ymd.year()));
    _month = static_cast<short>(static_cast<unsigned>(ymd.month()));
    _day = static_cast<short>(static_cast<unsigned>(ymd.day()));
}


void DateTime::computeDaytime()
{
    using namespace std::chrono;
    auto us = microseconds((_utcTime - GREG_EPOCH_OFFSET) / 10);
    auto tp = system_clock::time_point(duration_cast<system_clock::duration>(us));
    auto dp = floor<days>(tp);
    auto tod = tp - dp; // time since midnight

    auto h = duration_cast<hours>(tod);
    auto m = duration_cast<minutes>(tod - h);
    auto s = duration_cast<seconds>(tod - h - m);
    auto ms = duration_cast<milliseconds>(tod - h - m - s);
    auto us_frac = duration_cast<microseconds>(tod - h - m - s - ms);

    _hour = static_cast<short>(h.count());
    _minute = static_cast<short>(m.count());
    _second = static_cast<short>(s.count());
    _millisecond = static_cast<short>(ms.count());
    _microsecond = static_cast<short>(us_frac.count());
}


//
// LocalDateTime
//


LocalDateTime::LocalDateTime()
{
    determineTzd(true);
}


LocalDateTime::LocalDateTime(int year, int month, int day, int hour, int minute,
                             int second, int millisecond, int microsecond)
    : _dateTime(year, month, day, hour, minute, second, millisecond, microsecond)
{
    determineTzd();
}


LocalDateTime::LocalDateTime(int tzd, int year, int month, int day, int hour,
                             int minute, int second, int millisecond, int microsecond)
    : _dateTime(year, month, day, hour, minute, second, millisecond, microsecond)
    , _tzd(tzd)
{
}


LocalDateTime::LocalDateTime(double julianDay)
    : _dateTime(julianDay)
{
    determineTzd(true);
}


LocalDateTime::LocalDateTime(int tzd, double julianDay)
    : _dateTime(julianDay)
    , _tzd(tzd)
{
    adjustForTzd();
}


LocalDateTime::LocalDateTime(const DateTime& dateTime)
    : _dateTime(dateTime)
{
    determineTzd(true);
}


LocalDateTime::LocalDateTime(int tzd, const DateTime& dateTime)
    : _dateTime(dateTime)
    , _tzd(tzd)
{
    adjustForTzd();
}


LocalDateTime::LocalDateTime(int tzd, const DateTime& dateTime, bool adjust)
    : _dateTime(dateTime)
    , _tzd(tzd)
{
    if (adjust)
        adjustForTzd();
}


LocalDateTime::LocalDateTime(const LocalDateTime& dateTime)
    : _dateTime(dateTime._dateTime)
    , _tzd(dateTime._tzd)
{
}


LocalDateTime::LocalDateTime(Timestamp::UtcTimeVal utcTime,
                             Timestamp::TimeDiff diff, int tzd)
    : _dateTime(utcTime, diff)
    , _tzd(tzd)
{
    adjustForTzd();
}


LocalDateTime::~LocalDateTime()
{
}


LocalDateTime& LocalDateTime::operator=(const LocalDateTime& dateTime)
{
    if (&dateTime != this) {
        _dateTime = dateTime._dateTime;
        _tzd = dateTime._tzd;
    }
    return *this;
}


LocalDateTime& LocalDateTime::operator=(const Timestamp& timestamp)
{
    if (timestamp != this->timestamp()) {
        _dateTime = timestamp;
        determineTzd(true);
    }
    return *this;
}


LocalDateTime& LocalDateTime::operator=(double julianDay)
{
    _dateTime = julianDay;
    determineTzd(true);
    return *this;
}


LocalDateTime& LocalDateTime::assign(int year, int month, int day, int hour,
                                     int minute, int second, int millisecond,
                                     int microseconds)
{
    _dateTime.assign(year, month, day, hour, minute, second, millisecond, microseconds);
    determineTzd(false);
    return *this;
}


LocalDateTime& LocalDateTime::assign(int tzd, int year, int month, int day,
                                     int hour, int minute, int second,
                                     int millisecond, int microseconds)
{
    _dateTime.assign(year, month, day, hour, minute, second, millisecond, microseconds);
    _tzd = tzd;
    return *this;
}


LocalDateTime& LocalDateTime::assign(int tzd, double julianDay)
{
    _tzd = tzd;
    _dateTime = julianDay;
    adjustForTzd();
    return *this;
}


void LocalDateTime::swap(LocalDateTime& dateTime)
{
    _dateTime.swap(dateTime._dateTime);
    std::swap(_tzd, dateTime._tzd);
}


DateTime LocalDateTime::utc() const
{
    return DateTime(_dateTime.utcTime(),
                    -((Timestamp::TimeDiff)_tzd) * Timespan::SECONDS);
}


bool LocalDateTime::operator==(const LocalDateTime& dateTime) const
{
    return utcTime() == dateTime.utcTime();
}


bool LocalDateTime::operator!=(const LocalDateTime& dateTime) const
{
    return utcTime() != dateTime.utcTime();
}


bool LocalDateTime::operator<(const LocalDateTime& dateTime) const
{
    return utcTime() < dateTime.utcTime();
}


bool LocalDateTime::operator<=(const LocalDateTime& dateTime) const
{
    return utcTime() <= dateTime.utcTime();
}


bool LocalDateTime::operator>(const LocalDateTime& dateTime) const
{
    return utcTime() > dateTime.utcTime();
}


bool LocalDateTime::operator>=(const LocalDateTime& dateTime) const
{
    return utcTime() >= dateTime.utcTime();
}


LocalDateTime LocalDateTime::operator+(const Timespan& span) const
{
    // First calculate the adjusted UTC time, then calculate the
    // locally adjusted time by constructing a new LocalDateTime.
    DateTime tmp(utcTime(), span.totalMicroseconds());
    return LocalDateTime(tmp);
}


LocalDateTime LocalDateTime::operator-(const Timespan& span) const
{
    // First calculate the adjusted UTC time, then calculate the
    // locally adjusted time by constructing a new LocalDateTime.
    DateTime tmp(utcTime(), -span.totalMicroseconds());
    return LocalDateTime(tmp);
}


Timespan LocalDateTime::operator-(const LocalDateTime& dateTime) const
{
    return Timespan((utcTime() - dateTime.utcTime()) / 10);
}


LocalDateTime& LocalDateTime::operator+=(const Timespan& span)
{
    // Use the same trick as in operator+. Create a UTC time, adjust
    // it for the span, and convert back to LocalDateTime. This will
    // recalculate the tzd correctly in the case where the addition
    // crosses a DST boundary.
    *this = DateTime(utcTime(), span.totalMicroseconds());
    return *this;
}


LocalDateTime& LocalDateTime::operator-=(const Timespan& span)
{
    // Use the same trick as in operator-. Create a UTC time, adjust
    // it for the span, and convert back to LocalDateTime. This will
    // recalculate the tzd correctly in the case where the subtraction
    // crosses a DST boundary.
    *this = DateTime(utcTime(), -span.totalMicroseconds());
    return *this;
}


void LocalDateTime::determineTzd(bool adjust)
{
    if (adjust) {
        std::time_t epochTime = _dateTime.timestamp().epochTime();
#if defined(_WIN32)
        std::tm* broken = std::localtime(&epochTime);
        if (!broken)
            throw std::runtime_error("System error: Cannot get local time");
        _tzd = (Timezone::utcOffset() + ((broken->tm_isdst == 1) ? 3600 : 0));
#else
        std::tm broken;
        if (!localtime_r(&epochTime, &broken))
            throw std::runtime_error("System error: Cannot get local time");
        _tzd = (Timezone::utcOffset() + ((broken.tm_isdst == 1) ? 3600 : 0));
#endif
        adjustForTzd();
    } else {
        int dst;
        dstOffset(dst);
        _tzd = (Timezone::utcOffset() + dst);
    }
}


std::time_t LocalDateTime::dstOffset(int& dstOffset) const
{
    std::time_t local;
    std::tm broken;

    broken.tm_year = (_dateTime.year() - 1900);
    broken.tm_mon = (_dateTime.month() - 1);
    broken.tm_mday = _dateTime.day();
    broken.tm_hour = _dateTime.hour();
    broken.tm_min = _dateTime.minute();
    broken.tm_sec = _dateTime.second();
    broken.tm_isdst = -1;
    local = std::mktime(&broken);

    dstOffset = (broken.tm_isdst == 1) ? 3600 : 0;
    return local;
}


//
// Timezone
//


int Timezone::tzd()
{
    return utcOffset() + dst();
}


#if defined(_WIN32)


//
// Timezone: Win
//


int Timezone::utcOffset()
{
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    (void)dstFlag;
    return -tzInfo.Bias * 60;
}


int Timezone::dst()
{
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    (void)dstFlag;
    return dstFlag == TIME_ZONE_ID_DAYLIGHT ? -tzInfo.DaylightBias * 60 : 0;
}


bool Timezone::isDst(const Timestamp& timestamp)
{
    std::time_t time = timestamp.epochTime();
    struct std::tm* tms = std::localtime(&time);
    if (!tms)
        throw std::runtime_error(
            "System error: cannot get local time DST flag");
    return tms->tm_isdst > 0;
}


std::string Timezone::name()
{
    std::string result;
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    WCHAR* ptr = dstFlag == TIME_ZONE_ID_DAYLIGHT ? tzInfo.DaylightName
                                                  : tzInfo.StandardName;
    // #if defined(UNICODE)
    //     UnicodeConverter::toUTF8(ptr, result);
    // #else
    char buffer[256];
    DWORD rc = WideCharToMultiByte(CP_ACP, 0, ptr, -1, buffer, sizeof(buffer),
                                   nullptr, nullptr);
    if (rc)
        result = buffer;
    // #endif
    return result;
}


std::string Timezone::standardName()
{
    std::string result;
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    WCHAR* ptr = tzInfo.StandardName;
    (void)dstFlag;
    // #if defined(UNICODE)
    //     UnicodeConverter::toUTF8(ptr, result);
    // #else
    char buffer[256];
    DWORD rc = WideCharToMultiByte(CP_ACP, 0, ptr, -1, buffer, sizeof(buffer),
                                   nullptr, nullptr);
    if (rc)
        result = buffer;
    // #endif
    return result;
}


std::string Timezone::dstName()
{
    std::string result;
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    WCHAR* ptr = tzInfo.DaylightName;
    (void)dstFlag;
    // #if defined(UNICODE)
    //     UnicodeConverter::toUTF8(ptr, result);
    // #else
    char buffer[256];
    DWORD rc = WideCharToMultiByte(CP_ACP, 0, ptr, -1, buffer, sizeof(buffer),
                                   nullptr, nullptr);
    if (rc)
        result = buffer;
    // #endif
    return result;
}


#else


//
// Timezone: Unix
//

class Base_API TZInfo{
    public:
        TZInfo(){tzset();
}

int timeZone()
{
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(POCO_ANDROID) // no timezone global var
    std::time_t now = std::time(nullptr);
    struct std::tm t;
    gmtime_r(&now, &t);
    std::time_t utc = std::mktime(&t);
    return now - utc;
#elif defined(__CYGWIN__)
    return -_timezone;
#else
    return -timezone;
#endif
}

const char* name(bool dst)
{
    return tzname[dst ? 1 : 0];
}
}
;


static TZInfo tzInfo;


int Timezone::utcOffset()
{
    return tzInfo.timeZone();
}


int Timezone::dst()
{
    std::time_t now = std::time(nullptr);
    struct std::tm t;
    if (!localtime_r(&now, &t))
        throw std::runtime_error("System error: cannot get local time DST offset");
    return t.tm_isdst == 1 ? 3600 : 0;
}


bool Timezone::isDst(const Timestamp& timestamp)
{
    std::time_t time = timestamp.epochTime();
    struct std::tm* tms = std::localtime(&time);
    if (!tms)
        throw std::runtime_error("System error: cannot get local time DST flag");
    return tms->tm_isdst > 0;
}


std::string Timezone::name()
{
    return std::string(tzInfo.name(dst() != 0));
}


std::string Timezone::standardName()
{
    return std::string(tzInfo.name(false));
}


std::string Timezone::dstName()
{
    return std::string(tzInfo.name(true));
}


#endif


//
// DateTimeFormat
//


const std::string DateTimeFormat::ISO8601_FORMAT("%Y-%m-%dT%H:%M:%S%z");
const std::string DateTimeFormat::ISO8601_FRAC_FORMAT("%Y-%m-%dT%H:%M:%s%z");
const std::string DateTimeFormat::RFC822_FORMAT("%w, %e %b %y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC1123_FORMAT("%w, %e %b %Y %H:%M:%S %Z");
const std::string DateTimeFormat::HTTP_FORMAT("%w, %d %b %Y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC850_FORMAT("%W, %e-%b-%y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC1036_FORMAT("%W, %e %b %y %H:%M:%S %Z");
const std::string DateTimeFormat::ASCTIME_FORMAT("%w %b %f %H:%M:%S %Y");
const std::string DateTimeFormat::SORTABLE_FORMAT("%Y-%m-%d %H:%M:%S");


const std::string DateTimeFormat::WEEKDAY_NAMES[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"};


const std::string DateTimeFormat::MONTH_NAMES[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};


//
// DateTimeFormatter
//


void DateTimeFormatter::append(std::string& str, const LocalDateTime& dateTime,
                               std::string_view fmt)
{
    DateTimeFormatter::append(str, dateTime.utc(), fmt, dateTime.tzd());
}


void DateTimeFormatter::append(std::string& str, const DateTime& dateTime,
                               std::string_view fmt, int timeZoneDifferential)
{
    auto it = fmt.begin();
    auto end = fmt.end();
    while (it != end) {
        if (*it == '%') {
            if (++it != end) {
                switch (*it) {
                    case 'w':
                        str.append(DateTimeFormat::WEEKDAY_NAMES[dateTime.dayOfWeek()], 0, 3);
                        break;
                    case 'W':
                        str.append(
                            DateTimeFormat::WEEKDAY_NAMES[dateTime.dayOfWeek()]);
                        break;
                    case 'b':
                        str.append(
                            DateTimeFormat::MONTH_NAMES[dateTime.month() - 1],
                            0, 3);
                        break;
                    case 'B':
                        str.append(
                            DateTimeFormat::MONTH_NAMES[dateTime.month() - 1]);
                        break;
                    case 'd':
                        icy::numeric::format0(str, dateTime.day(), 2);
                        break;
                    case 'e':
                        icy::numeric::format(str, dateTime.day());
                        break;
                    case 'f':
                        icy::numeric::format(str, dateTime.day(), 2);
                        break;
                    case 'm':
                        icy::numeric::format0(str, dateTime.month(), 2);
                        break;
                    case 'n':
                        icy::numeric::format(str, dateTime.month());
                        break;
                    case 'o':
                        icy::numeric::format(str, dateTime.month(), 2);
                        break;
                    case 'y':
                        icy::numeric::format0(str, dateTime.year() % 100, 2);
                        break;
                    case 'Y':
                        icy::numeric::format0(str, dateTime.year(), 4);
                        break;
                    case 'H':
                        icy::numeric::format0(str, dateTime.hour(), 2);
                        break;
                    case 'h':
                        icy::numeric::format0(str, dateTime.hourAMPM(), 2);
                        break;
                    case 'a':
                        str.append(dateTime.isAM() ? "am" : "pm");
                        break;
                    case 'A':
                        str.append(dateTime.isAM() ? "AM" : "PM");
                        break;
                    case 'M':
                        icy::numeric::format0(str, dateTime.minute(), 2);
                        break;
                    case 'S':
                        icy::numeric::format0(str, dateTime.second(), 2);
                        break;
                    case 's':
                        icy::numeric::format0(str, dateTime.second(), 2);
                        str += '.';
                        icy::numeric::format0(str, dateTime.millisecond() * 1000 + dateTime.microsecond(), 6);
                        break;
                    case 'i':
                        icy::numeric::format0(str, dateTime.millisecond(), 3);
                        break;
                    case 'c':
                        icy::numeric::format(str, dateTime.millisecond() / 100);
                        break;
                    case 'F':
                        icy::numeric::format0(str, dateTime.millisecond() * 1000 + dateTime.microsecond(), 6);
                        break;
                    case 'z':
                        tzdISO(str, timeZoneDifferential);
                        break;
                    case 'Z':
                        tzdRFC(str, timeZoneDifferential);
                        break;
                    default:
                        str += *it;
                }
                ++it;
            }
        } else
            str += *it++;
    }
}


void DateTimeFormatter::append(std::string& str, const Timespan& timespan,
                               std::string_view fmt)
{
    auto it = fmt.begin();
    auto end = fmt.end();
    while (it != end) {
        if (*it == '%') {
            if (++it != end) {
                switch (*it) {
                    case 'd':
                        icy::numeric::format(str, timespan.days());
                        break;
                    case 'H':
                        icy::numeric::format0(str, timespan.hours(), 2);
                        break;
                    case 'h':
                        icy::numeric::format(str, timespan.totalHours());
                        break;
                    case 'M':
                        icy::numeric::format0(str, timespan.minutes(), 2);
                        break;
                    case 'm':
                        icy::numeric::format(str, timespan.totalMinutes());
                        break;
                    case 'S':
                        icy::numeric::format0(str, timespan.seconds(), 2);
                        break;
                    case 's':
                        icy::numeric::format(str, timespan.totalSeconds());
                        break;
                    case 'i':
                        icy::numeric::format0(str, timespan.milliseconds(), 3);
                        break;
                    case 'c':
                        icy::numeric::format(str, timespan.milliseconds() / 100);
                        break;
                    case 'F':
                        icy::numeric::format0(str, timespan.milliseconds() * 1000 + timespan.microseconds(), 6);
                        break;
                    default:
                        str += *it;
                }
                ++it;
            }
        } else
            str += *it++;
    }
}


void DateTimeFormatter::tzdISO(std::string& str, int timeZoneDifferential)
{
    if (timeZoneDifferential != UTC) {
        if (timeZoneDifferential >= 0) {
            str += '+';
            icy::numeric::format0(str, timeZoneDifferential / 3600, 2);
            str += ':';
            icy::numeric::format0(str, (timeZoneDifferential % 3600) / 60, 2);
        } else {
            str += '-';
            icy::numeric::format0(str, -timeZoneDifferential / 3600, 2);
            str += ':';
            icy::numeric::format0(str, (-timeZoneDifferential % 3600) / 60, 2);
        }
    } else
        str += 'Z';
}


void DateTimeFormatter::tzdRFC(std::string& str, int timeZoneDifferential)
{
    if (timeZoneDifferential != UTC) {
        if (timeZoneDifferential >= 0) {
            str += '+';
            icy::numeric::format0(str, timeZoneDifferential / 3600, 2);
            icy::numeric::format0(str, (timeZoneDifferential % 3600) / 60, 2);
        } else {
            str += '-';
            icy::numeric::format0(str, -timeZoneDifferential / 3600, 2);
            icy::numeric::format0(str, (-timeZoneDifferential % 3600) / 60, 2);
        }
    } else
        str += "GMT";
}


//
// DateTimeParser
//


#define SKIP_JUNK()                      \
    while (it != end && !::isdigit(*it)) \
    ++it


#define SKIP_DIGITS()                   \
    while (it != end && ::isdigit(*it)) \
    ++it


#define PARSE_NUMBER(var)               \
    while (it != end && ::isdigit(*it)) \
    var = var * 10 + ((*it++) - '0')


#define PARSE_NUMBER_N(var, n)                         \
    {                                                  \
        int i = 0;                                     \
        while (i++ < n && it != end && ::isdigit(*it)) \
            var = var * 10 + ((*it++) - '0');          \
    }


#define PARSE_FRACTIONAL_N(var, n)                     \
    {                                                  \
        int i = 0;                                     \
        while (i < n && it != end && ::isdigit(*it)) { \
            var = var * 10 + ((*it++) - '0');          \
            i++;                                       \
        }                                              \
        while (i++ < n)                                \
            var *= 10;                                 \
    }


void DateTimeParser::parse(std::string_view fmt, std::string_view str,
                           DateTime& dateTime, int& timeZoneDifferential)
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int millis = 0;
    int micros = 0;
    int tzd = 0;

    const char* it = str.data();
    const char* end = str.data() + str.size();
    const char* itf = fmt.data();
    const char* endf = fmt.data() + fmt.size();

    while (itf != endf && it != end) {
        if (*itf == '%') {
            if (++itf != endf) {
                switch (*itf) {
                    case 'w':
                    case 'W':
                        while (it != end && ::isspace(*it))
                            ++it;
                        while (it != end && ::isalpha(*it))
                            ++it;
                        break;
                    case 'b':
                    case 'B':
                        month = parseMonth(it, end);
                        break;
                    case 'd':
                    case 'e':
                    case 'f':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(day, 2);
                        break;
                    case 'm':
                    case 'n':
                    case 'o':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(month, 2);
                        break;
                    case 'y':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(year, 2);
                        if (year >= 69)
                            year += 1900;
                        else
                            year += 2000;
                        break;
                    case 'Y':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(year, 4);
                        break;
                    case 'r':
                        SKIP_JUNK();
                        PARSE_NUMBER(year);
                        if (year < 1000) {
                            if (year >= 69)
                                year += 1900;
                            else
                                year += 2000;
                        }
                        break;
                    case 'H':
                    case 'h':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(hour, 2);
                        break;
                    case 'a':
                    case 'A':
                        hour = parseAMPM(it, end, hour);
                        break;
                    case 'M':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(minute, 2);
                        break;
                    case 'S':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(second, 2);
                        break;
                    case 's':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(second, 2);
                        if (it != end && (*it == '.' || *it == ',')) {
                            ++it;
                            PARSE_FRACTIONAL_N(millis, 3);
                            PARSE_FRACTIONAL_N(micros, 3);
                            SKIP_DIGITS();
                        }
                        break;
                    case 'i':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(millis, 3);
                        break;
                    case 'c':
                        SKIP_JUNK();
                        PARSE_NUMBER_N(millis, 1);
                        millis *= 100;
                        break;
                    case 'F':
                        SKIP_JUNK();
                        PARSE_FRACTIONAL_N(millis, 3);
                        PARSE_FRACTIONAL_N(micros, 3);
                        SKIP_DIGITS();
                        break;
                    case 'z':
                    case 'Z':
                        tzd = parseTZD(it, end);
                        break;
                }
                ++itf;
            }
        } else
            ++itf;
    }
    if (month == 0)
        month = 1;
    if (day == 0)
        day = 1;
    if (DateTime::isValid(year, month, day, hour, minute, second, millis, micros))
        dateTime.assign(year, month, day, hour, minute, second, millis, micros);
    else
        throw std::runtime_error("Syntax error: date/time component out of range");
    timeZoneDifferential = tzd;
}


DateTime DateTimeParser::parse(std::string_view fmt, std::string_view str,
                               int& timeZoneDifferential)
{
    DateTime result;
    parse(fmt, str, result, timeZoneDifferential);
    return result;
}


bool DateTimeParser::tryParse(std::string_view fmt, std::string_view str,
                              DateTime& dateTime, int& timeZoneDifferential)
{
    try {
        parse(fmt, str, dateTime, timeZoneDifferential);
    } catch (std::exception&) {
        return false;
    }
    return true;
}


void DateTimeParser::parse(std::string_view str, DateTime& dateTime,
                           int& timeZoneDifferential)
{
    if (!tryParse(str, dateTime, timeZoneDifferential))
        throw std::runtime_error("Syntax error: Unsupported or invalid date/time format");
}


DateTime DateTimeParser::parse(std::string_view str,
                               int& timeZoneDifferential)
{
    DateTime result;
    if (tryParse(str, result, timeZoneDifferential))
        return result;
    else
        throw std::runtime_error("Syntax error: Unsupported or invalid date/time format");
}


bool DateTimeParser::tryParse(std::string_view str, DateTime& dateTime,
                              int& timeZoneDifferential)
{
    if (str.length() < 4)
        return false;

    if (str[3] == ',')
        return tryParse("%w, %e %b %r %H:%M:%S %Z", str, dateTime,
                        timeZoneDifferential);
    else if (str[3] == ' ')
        return tryParse(DateTimeFormat::ASCTIME_FORMAT, str, dateTime,
                        timeZoneDifferential);
    else if (str.find(',') < 10)
        return tryParse("%W, %e %b %r %H:%M:%S %Z", str, dateTime,
                        timeZoneDifferential);
    else if (::isdigit(str[0])) {
        if (str.find(' ') != std::string_view::npos || str.length() == 10)
            return tryParse(DateTimeFormat::SORTABLE_FORMAT, str, dateTime,
                            timeZoneDifferential);
        else if (str.find('.') != std::string_view::npos ||
                 str.find(',') != std::string_view::npos)
            return tryParse(DateTimeFormat::ISO8601_FRAC_FORMAT, str, dateTime,
                            timeZoneDifferential);
        else
            return tryParse(DateTimeFormat::ISO8601_FORMAT, str, dateTime,
                            timeZoneDifferential);
    } else
        return false;
}


int DateTimeParser::parseTZD(const char*& it,
                             const char* end)
{
    struct Zone
    {
        const char* designator;
        int timeZoneDifferential;
    };

    static Zone zones[] = {{"Z", 0},
                           {"UT", 0},
                           {"GMT", 0},
                           {"BST", 1 * 3600},
                           {"IST", 1 * 3600},
                           {"WET", 0},
                           {"WEST", 1 * 3600},
                           {"CET", 1 * 3600},
                           {"CEST", 2 * 3600},
                           {"EET", 2 * 3600},
                           {"EEST", 3 * 3600},
                           {"MSK", 3 * 3600},
                           {"MSD", 4 * 3600},
                           {"NST", -3 * 3600 - 1800},
                           {"NDT", -2 * 3600 - 1800},
                           {"AST", -4 * 3600},
                           {"ADT", -3 * 3600},
                           {"EST", -5 * 3600},
                           {"EDT", -4 * 3600},
                           {"CST", -6 * 3600},
                           {"CDT", -5 * 3600},
                           {"MST", -7 * 3600},
                           {"MDT", -6 * 3600},
                           {"PST", -8 * 3600},
                           {"PDT", -7 * 3600},
                           {"AKST", -9 * 3600},
                           {"AKDT", -8 * 3600},
                           {"HST", -10 * 3600},
                           {"AEST", 10 * 3600},
                           {"AEDT", 11 * 3600},
                           {"ACST", 9 * 3600 + 1800},
                           {"ACDT", 10 * 3600 + 1800},
                           {"AWST", 8 * 3600},
                           {"AWDT", 9 * 3600}};

    int tzd = 0;
    while (it != end && ::isspace(*it))
        ++it;
    if (it != end) {
        if (::isalpha(*it)) {
            std::string designator;
            designator += *it++;
            if (it != end && ::isalpha(*it))
                designator += *it++;
            if (it != end && ::isalpha(*it))
                designator += *it++;
            if (it != end && ::isalpha(*it))
                designator += *it++;
            for (unsigned i = 0; i < sizeof(zones) / sizeof(Zone); ++i) {
                if (designator == zones[i].designator) {
                    tzd = zones[i].timeZoneDifferential;
                    break;
                }
            }
        }
        if (it != end && (*it == '+' || *it == '-')) {
            int sign = *it == '+' ? 1 : -1;
            ++it;
            int hours = 0;
            PARSE_NUMBER_N(hours, 2);
            if (it != end && *it == ':')
                ++it;
            int minutes = 0;
            PARSE_NUMBER_N(minutes, 2);
            tzd += sign * (hours * 3600 + minutes * 60);
        }
    }
    return tzd;
}


int DateTimeParser::parseMonth(const char*& it,
                               const char* end)
{
    std::string month;
    while (it != end && (::isspace(*it) || ::ispunct(*it)))
        ++it;
    bool isFirst = true;
    while (it != end && ::isalpha(*it)) {
        char ch = (*it++);
        if (isFirst) {
            month += static_cast<char>(::toupper(ch));
            isFirst = false;
        } else
            month += static_cast<char>(::tolower(ch));
    }
    if (month.length() < 3)
        throw std::runtime_error("Syntax error: Month name must be at least "
                                 "three characters long: " +
                                 month);
    for (int i = 0; i < 12; ++i) {
        if (DateTimeFormat::MONTH_NAMES[i].find(month) == 0)
            return i + 1;
    }
    throw std::runtime_error("Syntax error: Not a valid month name: " + month);
}


int DateTimeParser::parseDayOfWeek(const char*& it,
                                   const char* end)
{
    std::string dow;
    while (it != end && (::isspace(*it) || ::ispunct(*it)))
        ++it;
    bool isFirst = true;
    while (it != end && ::isalpha(*it)) {
        char ch = (*it++);
        if (isFirst) {
            dow += static_cast<char>(::toupper(ch));
            isFirst = false;
        } else
            dow += static_cast<char>(::tolower(ch));
    }
    if (dow.length() < 3)
        throw std::runtime_error("Syntax error: Weekday name must be at least "
                                 "three characters long: " +
                                 dow);
    for (int i = 0; i < 7; ++i) {
        if (DateTimeFormat::WEEKDAY_NAMES[i].find(dow) == 0)
            return i;
    }
    throw std::runtime_error("Syntax error: Not a valid weekday name: " + dow);
}


int DateTimeParser::parseAMPM(const char*& it,
                              const char* end, int hour)
{
    std::string ampm;
    while (it != end && (::isspace(*it) || ::ispunct(*it)))
        ++it;
    while (it != end && ::isalpha(*it)) {
        char ch = (*it++);
        ampm += static_cast<char>(::toupper(ch));
    }
    if (ampm == "AM") {
        if (hour == 12)
            return 0;
        else
            return hour;
    } else if (ampm == "PM") {
        if (hour < 12)
            return hour + 12;
        else
            return hour;
    } else
        throw std::runtime_error(
            "Syntax error: Not a valid AM/PM designator: " + ampm);
}


//
// Timestamp
//


Timestamp::Timestamp()
{
    update();
}


Timestamp::Timestamp(TimeVal tv)
{
    _ts = tv;
}


Timestamp::Timestamp(const Timestamp& other)
{
    _ts = other._ts;
}


Timestamp::~Timestamp()
{
}


Timestamp& Timestamp::operator=(const Timestamp& other)
{
    _ts = other._ts;
    return *this;
}


Timestamp& Timestamp::operator=(TimeVal tv)
{
    _ts = tv;
    return *this;
}


void Timestamp::swap(Timestamp& timestamp)
{
    std::swap(_ts, timestamp._ts);
}


Timestamp Timestamp::fromEpochTime(std::time_t t)
{
    return Timestamp(TimeVal(t) * resolution());
}


Timestamp Timestamp::fromUtcTime(UtcTimeVal val)
{
    val -= (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
    val /= 10;
    return Timestamp(val);
}


void Timestamp::update()
{
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    _ts = std::chrono::duration_cast<std::chrono::microseconds>(epoch).count();
}


//
// Timespan
//


const Timespan::TimeDiff Timespan::MILLISECONDS = 1000;
const Timespan::TimeDiff Timespan::SECONDS = 1000 * Timespan::MILLISECONDS;
const Timespan::TimeDiff Timespan::MINUTES = 60 * Timespan::SECONDS;
const Timespan::TimeDiff Timespan::HOURS = 60 * Timespan::MINUTES;
const Timespan::TimeDiff Timespan::DAYS = 24 * Timespan::HOURS;


Timespan::Timespan()
    : _span(0)
{
}


Timespan::Timespan(TimeDiff microseconds)
    : _span(microseconds)
{
}


Timespan::Timespan(long seconds, long microseconds)
    : _span(TimeDiff(seconds) * SECONDS + microseconds)
{
}


Timespan::Timespan(int days, int hours, int minutes, int seconds, int microseconds)
    : _span(TimeDiff(microseconds) +
            TimeDiff(seconds) * SECONDS +
            TimeDiff(minutes) * MINUTES +
            TimeDiff(hours) * HOURS +
            TimeDiff(days) * DAYS)
{
}


Timespan::Timespan(const Timespan& timespan)
    : _span(timespan._span)
{
}


Timespan::~Timespan()
{
}


Timespan& Timespan::operator=(const Timespan& timespan)
{
    _span = timespan._span;
    return *this;
}


Timespan& Timespan::operator=(TimeDiff microseconds)
{
    _span = microseconds;
    return *this;
}


Timespan& Timespan::assign(int days, int hours, int minutes, int seconds,
                           int microseconds)
{
    _span = TimeDiff(microseconds) + TimeDiff(seconds) * SECONDS +
            TimeDiff(minutes) * MINUTES + TimeDiff(hours) * HOURS +
            TimeDiff(days) * DAYS;
    return *this;
}


Timespan& Timespan::assign(long seconds, long microseconds)
{
    _span = TimeDiff(seconds) * SECONDS + TimeDiff(microseconds);
    return *this;
}


void Timespan::swap(Timespan& timespan)
{
    std::swap(_span, timespan._span);
}


Timespan Timespan::operator+(const Timespan& d) const
{
    return Timespan(_span + d._span);
}


Timespan Timespan::operator-(const Timespan& d) const
{
    return Timespan(_span - d._span);
}


Timespan& Timespan::operator+=(const Timespan& d)
{
    _span += d._span;
    return *this;
}


Timespan& Timespan::operator-=(const Timespan& d)
{
    _span -= d._span;
    return *this;
}


Timespan Timespan::operator+(TimeDiff microseconds) const
{
    return Timespan(_span + microseconds);
}


Timespan Timespan::operator-(TimeDiff microseconds) const
{
    return Timespan(_span - microseconds);
}


Timespan& Timespan::operator+=(TimeDiff microseconds)
{
    _span += microseconds;
    return *this;
}


Timespan& Timespan::operator-=(TimeDiff microseconds)
{
    _span -= microseconds;
    return *this;
}


//
// Stopwatch
//


Stopwatch::Stopwatch()
    : _elapsed(0)
    , _running(false)
{
}


Stopwatch::~Stopwatch()
{
}


void Stopwatch::start()
{
    if (!_running) {
        _start.update();
        _running = true;
    }
}


void Stopwatch::stop()
{
    if (_running) {
        Timestamp current;
        _elapsed += current - _start;
        _running = false;
    }
}


int Stopwatch::elapsedSeconds() const
{
    return int(elapsed() / resolution());
}


int Stopwatch::elapsedMilliseconds() const
{
    return int(elapsed() / (resolution() / 1000));
}


Timestamp::TimeVal Stopwatch::resolution()
{
    return Timestamp::resolution();
}


Timestamp::TimeDiff Stopwatch::elapsed() const
{
    if (_running) {
        Timestamp current;
        return _elapsed + (current - _start);
    } else {
        return _elapsed;
    }
}


void Stopwatch::reset()
{
    _elapsed = 0;
    _running = false;
}


void Stopwatch::restart()
{
    _elapsed = 0;
    _start.update();
    _running = true;
}


} // namespace icy


/// @}
