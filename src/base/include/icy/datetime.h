///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/packetsignal.h"
#include <ctime>
#include <string_view>


namespace icy {


/// A Timestamp stores a monotonic* time value
/// with (theoretical) microseconds resolution.
/// Timestamps can be compared with each other
/// and simple arithmetics are supported.
///
/// [*] Note that Timestamp values are only monotonic as
/// long as the systems's clock is monotonic as well
/// (and not, e.g. set back).
///
/// Timestamps are UTC (Coordinated Universal Time)
/// based and thus independent of the timezone
/// in effect on the system.
class Base_API Timestamp
{
public:
    using TimeVal = std::int64_t;    ///< monotonic UTC time value in microsecond resolution
    using UtcTimeVal = std::int64_t; ///< monotonic UTC time value in 100 nanosecond resolution
    using TimeDiff = std::int64_t;   ///< difference between two timestamps in microseconds

    /// Creates a timestamp with the current time.
    Timestamp();

    /// Creates a timestamp from the given time value.
    Timestamp(TimeVal tv);

    /// Copy constructor.
    Timestamp(const Timestamp& other);

    /// Destroys the timestamp
    ~Timestamp();

    Timestamp& operator=(const Timestamp& other);
    Timestamp& operator=(TimeVal tv);

    /// Swaps the Timestamp with another one.
    void swap(Timestamp& timestamp);

    /// Updates the Timestamp with the current time.
    void update();

    bool operator==(const Timestamp& ts) const;
    bool operator!=(const Timestamp& ts) const;
    bool operator>(const Timestamp& ts) const;
    bool operator>=(const Timestamp& ts) const;
    bool operator<(const Timestamp& ts) const;
    bool operator<=(const Timestamp& ts) const;

    Timestamp operator+(TimeDiff d) const;
    Timestamp operator-(TimeDiff d) const;
    TimeDiff operator-(const Timestamp& ts) const;
    Timestamp& operator+=(TimeDiff d);
    Timestamp& operator-=(TimeDiff d);

    /// Returns the timestamp expressed in time_t.
    /// time_t base time is midnight, January 1, 1970.
    /// Resolution is one second.
    std::time_t epochTime() const;

    /// Returns the timestamp expressed in UTC-based
    /// time. UTC base time is midnight, October 15, 1582.
    /// Resolution is 100 nanoseconds.
    UtcTimeVal utcTime() const;

    /// Returns the timestamp expressed in microseconds
    /// since the Unix epoch, midnight, January 1, 1970.
    TimeVal epochMicroseconds() const;

    /// Returns the time elapsed since the time denoted by
    /// the timestamp. Equivalent to Timestamp() - *this.
    TimeDiff elapsed() const;

    /// Returns true iff the given interval has passed
    /// since the time denoted by the timestamp.
    bool isElapsed(TimeDiff interval) const;

    /// Creates a timestamp from a std::time_t.
    static Timestamp fromEpochTime(std::time_t t);

    /// Creates a timestamp from a UTC time value.
    static Timestamp fromUtcTime(UtcTimeVal val);

    /// Returns the resolution in units per second.
    /// Since the timestamp has microsecond resolution,
    /// the returned value is always 1000000.
    static TimeVal resolution();

private:
    TimeVal _ts;
};


// ---------------------------------------------------------------------
//

/// A class that represents time spans up to microsecond resolution.
class Base_API Timespan
{
public:
    using TimeDiff = Timestamp::TimeDiff;

    /// Creates a zero Timespan.
    Timespan();

    /// Creates a Timespan.
    Timespan(TimeDiff microseconds);

    /// Creates a Timespan. Useful for creating
    /// a Timespan from a struct timeval.
    Timespan(long seconds, long microseconds);

    /// Creates a Timespan.
    Timespan(int days, int hours, int minutes, int seconds, int microseconds);

    /// Creates a Timespan from another one.
    Timespan(const Timespan& timespan);

    /// Destroys the Timespan.
    ~Timespan();

    /// Assignment operator.
    Timespan& operator=(const Timespan& timespan);

    /// Assignment operator.
    Timespan& operator=(TimeDiff microseconds);

    /// Assigns a new span.
    Timespan& assign(int days, int hours, int minutes, int seconds, int microseconds);

    /// Assigns a new span. Useful for assigning
    /// from a struct timeval.
    Timespan& assign(long seconds, long microseconds);

    /// Swaps the Timespan with another one.
    void swap(Timespan& timespan);

    bool operator==(const Timespan& ts) const;
    bool operator!=(const Timespan& ts) const;
    bool operator>(const Timespan& ts) const;
    bool operator>=(const Timespan& ts) const;
    bool operator<(const Timespan& ts) const;
    bool operator<=(const Timespan& ts) const;

    bool operator==(TimeDiff microseconds) const;
    bool operator!=(TimeDiff microseconds) const;
    bool operator>(TimeDiff microseconds) const;
    bool operator>=(TimeDiff microseconds) const;
    bool operator<(TimeDiff microseconds) const;
    bool operator<=(TimeDiff microseconds) const;

    Timespan operator+(const Timespan& d) const;
    Timespan operator-(const Timespan& d) const;
    Timespan& operator+=(const Timespan& d);
    Timespan& operator-=(const Timespan& d);

    Timespan operator+(TimeDiff microseconds) const;
    Timespan operator-(TimeDiff microseconds) const;
    Timespan& operator+=(TimeDiff microseconds);
    Timespan& operator-=(TimeDiff microseconds);

    /// Returns the number of days.
    int days() const;

    /// Returns the number of hours (0 to 23).
    int hours() const;

    /// Returns the total number of hours.
    int totalHours() const;

    /// Returns the number of minutes (0 to 59).
    int minutes() const;

    /// Returns the total number of minutes.
    int totalMinutes() const;

    /// Returns the number of seconds (0 to 59).
    int seconds() const;

    /// Returns the total number of seconds.
    int totalSeconds() const;

    /// Returns the number of milliseconds (0 to 999).
    int milliseconds() const;

    /// Returns the total number of milliseconds.
    TimeDiff totalMilliseconds() const;

    /// Returns the fractions of a millisecond
    /// in microseconds (0 to 999).
    int microseconds() const;

    /// Returns the fractions of a second
    /// in microseconds (0 to 999999).
    int useconds() const;

    /// Returns the total number of microseconds.
    TimeDiff totalMicroseconds() const;

    static const TimeDiff MILLISECONDS; ///< The number of microseconds in a millisecond.
    static const TimeDiff SECONDS;      ///< The number of microseconds in a second.
    static const TimeDiff MINUTES;      ///< The number of microseconds in a minute.
    static const TimeDiff HOURS;        ///< The number of microseconds in a hour.
    static const TimeDiff DAYS;         ///< The number of microseconds in a day.

private:
    TimeDiff _span;
};


// ---------------------------------------------------------------------
//

/// This class represents an instant in time, expressed
/// in years, months, days, hours, minutes, seconds
/// and milliseconds based on the Gregorian calendar.
/// The class is mainly useful for conversions between
/// UTC, Julian day and Gregorian calendar dates.
///
/// The date and time stored in a DateTime is always in UTC
/// (Coordinated Universal Time) and thus independent of the
/// timezone in effect on the system.
///
/// Conversion calculations are based on algorithms
/// collected and described by Peter Baum at
/// http://vsg.cape.com/~pbaum/date/date0.htm
///
/// Internally, this class stores a date/time in two
/// forms (UTC and broken down) for performance reasons. Only use
/// this class for conversions between date/time representations.
/// Use the Timestamp class for everything else.
///
/// Notes:
///   * Zero is a valid year (in accordance with ISO 8601 and astronomical year
///   numbering)
///   * Year zero (0) is a leap year
///   * Negative years (years preceding 1 BC) are not supported
///
/// For more information, please see:
///   * http://en.wikipedia.org/wiki/Gregorian_Calendar
///   * http://en.wikipedia.org/wiki/Julian_day
///   * http://en.wikipedia.org/wiki/UTC
///   * http://en.wikipedia.org/wiki/ISO_8601
class Base_API DateTime
{
public:
    /// Symbolic names for month numbers (1 to 12).
    enum Months
    {
        JANUARY = 1,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER
    };

    /// Symbolic names for week day numbers (0 to 6).
    enum DaysOfWeek
    {
        SUNDAY = 0,
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY
    };

    /// Creates a DateTime for the current date and time.
    DateTime();

    /// Creates a DateTime for the date and time given in
    /// a Timestamp.
    DateTime(const Timestamp& timestamp);

    /// Creates a DateTime for the given Gregorian date and time.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 59.
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    DateTime(int year, int month, int day, int hour = 0, int minute = 0,
             int second = 0, int millisecond = 0, int microsecond = 0);

    /// Creates a DateTime for the given Julian day.
    DateTime(double julianDay);

    /// Creates a DateTime from an UtcTimeVal and a TimeDiff.
    ///
    /// Mainly used internally by DateTime and friends.
    DateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff);

    /// Copy constructor. Creates the DateTime from another one.
    DateTime(const DateTime& dateTime);

    /// Destroys the DateTime.
    ~DateTime();

    /// Assigns another DateTime.
    DateTime& operator=(const DateTime& dateTime);

    /// Assigns a Timestamp.
    DateTime& operator=(const Timestamp& timestamp);

    /// Assigns a Julian day.
    DateTime& operator=(double julianDay);

    /// Assigns a Gregorian date and time.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 59.
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    DateTime& assign(int year, int month, int day, int hour = 0, int minute = 0,
                     int second = 0, int millisecond = 0, int microseconds = 0);

    /// Swaps the DateTime with another one.
    void swap(DateTime& dateTime);

    /// Returns the year.
    int year() const;

    /// Returns the month (1 to 12).
    int month() const;

    /// Returns the week number within the year.
    /// FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
    /// The returned week number will be from 0 to 53. Week number 1 is the week
    /// containing January 4. This is in accordance to ISO 8601.
    ///
    /// The following example assumes that firstDayOfWeek is MONDAY. For 2005,
    /// which started
    /// on a Saturday, week 1 will be the week starting on Monday, January 3.
    /// January 1 and 2 will fall within week 0 (or the last week of the
    /// previous year).
    ///
    /// For 2007, which starts on a Monday, week 1 will be the week startung on
    /// Monday, January 1.
    /// There will be no week 0 in 2007.
    int week(int firstDayOfWeek = MONDAY) const;

    /// Returns the day witin the month (1 to 31).
    int day() const;

    /// Returns the weekday (0 to 6, where
    /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
    int dayOfWeek() const;

    /// Returns the number of the day in the year.
    /// January 1 is 1, February 1 is 32, etc.
    int dayOfYear() const;

    /// Returns the hour (0 to 23).
    int hour() const;

    /// Returns the hour (0 to 12).
    int hourAMPM() const;

    /// Returns true if hour < 12;
    bool isAM() const;

    /// Returns true if hour >= 12.
    bool isPM() const;

    /// Returns the minute (0 to 59).
    int minute() const;

    /// Returns the second (0 to 59).
    int second() const;

    /// Returns the millisecond (0 to 999)
    int millisecond() const;

    /// Returns the microsecond (0 to 999)
    int microsecond() const;

    /// Returns the julian day for the date and time.
    double julianDay() const;

    /// Returns the date and time expressed as a Timestamp.
    Timestamp timestamp() const;

    /// Returns the date and time expressed in UTC-based
    /// time. UTC base time is midnight, October 15, 1582.
    /// Resolution is 100 nanoseconds.
    Timestamp::UtcTimeVal utcTime() const;

    bool operator==(const DateTime& dateTime) const;
    bool operator!=(const DateTime& dateTime) const;
    bool operator<(const DateTime& dateTime) const;
    bool operator<=(const DateTime& dateTime) const;
    bool operator>(const DateTime& dateTime) const;
    bool operator>=(const DateTime& dateTime) const;

    DateTime operator+(const Timespan& span) const;
    DateTime operator-(const Timespan& span) const;
    Timespan operator-(const DateTime& dateTime) const;
    DateTime& operator+=(const Timespan& span);
    DateTime& operator-=(const Timespan& span);

    /// Converts a local time into UTC, by applying the given time zone
    /// differential.
    void makeUTC(int tzd);

    /// Converts a UTC time into a local time, by applying the given time zone
    /// differential.
    void makeLocal(int tzd);

    /// Returns true if the given year is a leap year;
    /// false otherwise.
    static bool isLeapYear(int year);

    /// Returns the number of days in the given month
    /// and year. Month is from 1 to 12.
    static int daysOfMonth(int year, int month);

    /// Checks if the given date and time is valid
    /// (all arguments are within a proper range).
    ///
    /// Returns true if all arguments are valid, false otherwise.
    static bool isValid(int year, int month, int day, int hour = 0,
                        int minute = 0, int second = 0, int millisecond = 0,
                        int microsecond = 0);


protected:
    /// Computes the Julian day for an UTC time.
    static double toJulianDay(Timestamp::UtcTimeVal utcTime);

    /// Computes the Julian day for a gregorian calendar date and time.
    /// See <http://vsg.cape.com/~pbaum/date/jdimp.htm>, section 2.3.1 for the
    /// algorithm.
    static double toJulianDay(int year, int month, int day, int hour = 0,
                              int minute = 0, int second = 0,
                              int millisecond = 0, int microsecond = 0);

    /// Computes the UTC time for a Julian day.
    static Timestamp::UtcTimeVal toUtcTime(double julianDay);

    /// Computes the Gregorian date for the given Julian day.
    /// See <http://vsg.cape.com/~pbaum/date/injdimp.htm>, section 3.3.1 for the
    /// algorithm.
    void computeGregorian(double julianDay);

    /// Extracts the daytime (hours, minutes, seconds, etc.) from the stored
    /// utcTime.
    void computeDaytime();

private:
    /// Utility functions used to correct the overflow in computeGregorian
    void checkLimit(short& lower, short& higher, short limit);
    void normalize();

    Timestamp::UtcTimeVal _utcTime;
    short _year;
    short _month;
    short _day;
    short _hour;
    short _minute;
    short _second;
    short _millisecond;
    short _microsecond;
};


//
// DateTime inlines
//


inline Timestamp DateTime::timestamp() const
{
    return Timestamp::fromUtcTime(_utcTime);
}


inline Timestamp::UtcTimeVal DateTime::utcTime() const
{
    return _utcTime;
}


inline int DateTime::year() const
{
    return _year;
}


inline int DateTime::month() const
{
    return _month;
}


inline int DateTime::day() const
{
    return _day;
}


inline int DateTime::hour() const
{
    return _hour;
}


inline int DateTime::hourAMPM() const
{
    if (_hour < 1)
        return 12;
    else if (_hour > 12)
        return _hour - 12;
    else
        return _hour;
}


inline bool DateTime::isAM() const
{
    return _hour < 12;
}


inline bool DateTime::isPM() const
{
    return _hour >= 12;
}


inline int DateTime::minute() const
{
    return _minute;
}


inline int DateTime::second() const
{
    return _second;
}


inline int DateTime::millisecond() const
{
    return _millisecond;
}


inline int DateTime::microsecond() const
{
    return _microsecond;
}


inline bool DateTime::operator==(const DateTime& dateTime) const
{
    return _utcTime == dateTime._utcTime;
}


inline bool DateTime::operator!=(const DateTime& dateTime) const
{
    return _utcTime != dateTime._utcTime;
}


inline bool DateTime::operator<(const DateTime& dateTime) const
{
    return _utcTime < dateTime._utcTime;
}


inline bool DateTime::operator<=(const DateTime& dateTime) const
{
    return _utcTime <= dateTime._utcTime;
}


inline bool DateTime::operator>(const DateTime& dateTime) const
{
    return _utcTime > dateTime._utcTime;
}


inline bool DateTime::operator>=(const DateTime& dateTime) const
{
    return _utcTime >= dateTime._utcTime;
}


inline bool DateTime::isLeapYear(int year)
{
    return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}


/// Exchange two DateTime values.
inline void swap(DateTime& d1, DateTime& d2)
{
    d1.swap(d2);
}


// ---------------------------------------------------------------------
//

/// This class provides information about the current timezone.
class Base_API Timezone
{
public:
    /// Returns the offset of local time to UTC, in seconds.
    ///     local time = UTC + utcOffset() + dst().
    static int utcOffset();

    /// Returns the daylight saving time offset in seconds if
    /// daylight saving time is in use.
    ///     local time = UTC + utcOffset() + dst().
    static int dst();

    /// Returns true if daylight saving time is in effect
    /// for the given time. Depending on the operating system
    /// platform this might only work reliably for certain
    /// date ranges, as the C library's localtime() function
    /// is used.
    static bool isDst(const Timestamp& timestamp);

    /// Returns the time zone differential for the current timezone.
    /// The timezone differential is computed as utcOffset() + dst()    /// and
    /// is expressed in seconds.
    static int tzd();

    /// Returns the timezone name currently in effect.
    static std::string name();

    /// Returns the timezone name if not daylight saving time is in effect.
    static std::string standardName();

    /// Returns the timezone name if daylight saving time is in effect.
    static std::string dstName();
};


// ---------------------------------------------------------------------
//

/// This class represents an instant in local time
/// (as opposed to UTC), expressed in years, months, days,
/// hours, minutes, seconds and milliseconds based on the
/// Gregorian calendar.
///
/// In addition to the date and time, the class also
/// maintains a time zone differential, which denotes
/// the difference in seconds from UTC to local time,
/// i.e. UTC = local time - time zone differential.
///
/// Although LocalDateTime supports relational and arithmetic
/// operators, all date/time comparisons and date/time arithmetics
/// should be done in UTC, using the DateTime or Timestamp
/// class for better performance. The relational operators
/// normalize the dates/times involved to UTC before carrying out
/// the comparison.
///
/// The time zone differential is based on the input date and
/// time and current time zone. A number of constructors accept
/// an explicit time zone differential parameter. These should
/// not be used since daylight savings time processing is impossible
/// since the time zone is unknown. Each of the constructors
/// accepting a tzd parameter have been marked as deprecated and
/// may be removed in a future revision.
class Base_API LocalDateTime
{
public:
    /// Creates a LocalDateTime with the current date/time
    /// for the current time zone.
    LocalDateTime();

    /// Creates a LocalDateTime for the given Gregorian local date and time.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 59.
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    LocalDateTime(int year, int month, int day, int hour = 0, int minute = 0,
                  int second = 0, int millisecond = 0, int microsecond = 0);

    ///@ deprecated
    /// Creates a LocalDateTime for the given Gregorian date and time in the
    /// time zone denoted by the time zone differential in tzd.
    ///   * tzd is in seconds.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 59.
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    LocalDateTime(int tzd, int year, int month, int day, int hour, int minute,
                  int second, int millisecond, int microsecond);

    /// Creates a LocalDateTime from the UTC time given in dateTime,
    /// using the time zone differential of the current time zone.
    LocalDateTime(const DateTime& dateTime);

    ///@ deprecated
    /// Creates a LocalDateTime from the UTC time given in dateTime,
    /// using the given time zone differential. Adjusts dateTime
    /// for the given time zone differential.
    LocalDateTime(int tzd, const DateTime& dateTime);


    ///@ deprecated
    /// Creates a LocalDateTime from the UTC time given in dateTime,
    /// using the given time zone differential. If adjust is true,
    /// adjusts dateTime for the given time zone differential.
    LocalDateTime(int tzd, const DateTime& dateTime, bool adjust);

    /// Creates a LocalDateTime for the given Julian day in the local time zone.
    LocalDateTime(double julianDay);


    ///@ deprecated
    /// Creates a LocalDateTime for the given Julian day in the time zone
    /// denoted by the time zone differential in tzd.
    LocalDateTime(int tzd, double julianDay);

    /// Copy constructor. Creates the LocalDateTime from another one.
    LocalDateTime(const LocalDateTime& dateTime);

    /// Destroys the LocalDateTime.
    ~LocalDateTime();

    /// Assigns another LocalDateTime.
    LocalDateTime& operator=(const LocalDateTime& dateTime);

    /// Assigns a timestamp
    LocalDateTime& operator=(const Timestamp& timestamp);

    /// Assigns a Julian day in the local time zone.
    LocalDateTime& operator=(double julianDay);

    /// Assigns a Gregorian local date and time.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 59.
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    LocalDateTime& assign(int year, int month, int day, int hour = 0,
                          int minute = 0, int second = 0, int millisecond = 0,
                          int microseconds = 0);

    ///@ deprecated
    /// Assigns a Gregorian local date and time in the time zone denoted by
    /// the time zone differential in tzd.
    ///   * tzd is in seconds.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 59.
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    LocalDateTime& assign(int tzd, int year, int month, int day, int hour,
                          int minute, int second, int millisecond,
                          int microseconds);

    ///@ deprecated
    /// Assigns a Julian day in the time zone denoted by the
    /// time zone differential in tzd.
    LocalDateTime& assign(int tzd, double julianDay);

    /// Swaps the LocalDateTime with another one.
    void swap(LocalDateTime& dateTime);

    /// Returns the year.
    int year() const;

    /// Returns the month (1 to 12).
    int month() const;

    /// Returns the week number within the year.
    /// FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
    /// The returned week number will be from 0 to 53. Week number 1 is the week
    /// containing January 4. This is in accordance to ISO 8601.
    ///
    /// The following example assumes that firstDayOfWeek is MONDAY. For 2005,
    /// which started
    /// on a Saturday, week 1 will be the week starting on Monday, January 3.
    /// January 1 and 2 will fall within week 0 (or the last week of the
    /// previous year).
    ///
    /// For 2007, which starts on a Monday, week 1 will be the week startung on
    /// Monday, January 1.
    /// There will be no week 0 in 2007.
    int week(int firstDayOfWeek = DateTime::MONDAY) const;

    /// Returns the day witin the month (1 to 31).
    int day() const;

    /// Returns the weekday (0 to 6, where
    /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
    int dayOfWeek() const;

    /// Returns the number of the day in the year.
    /// January 1 is 1, February 1 is 32, etc.
    int dayOfYear() const;

    /// Returns the hour (0 to 23).
    int hour() const;

    /// Returns the hour (0 to 12).
    int hourAMPM() const;

    /// Returns true if hour < 12;
    bool isAM() const;

    /// Returns true if hour >= 12.
    bool isPM() const;

    /// Returns the minute (0 to 59).
    int minute() const;

    /// Returns the second (0 to 59).
    int second() const;

    /// Returns the millisecond (0 to 999)
    int millisecond() const;

    /// Returns the microsecond (0 to 999)
    int microsecond() const;

    /// Returns the julian day for the date.
    double julianDay() const;

    /// Returns the time zone differential.
    int tzd() const;

    /// Returns the UTC equivalent for the local date and time.
    DateTime utc() const;

    /// Returns the date and time expressed as a Timestamp.
    Timestamp timestamp() const;

    /// Returns the UTC equivalent for the local date and time.
    Timestamp::UtcTimeVal utcTime() const;

    bool operator==(const LocalDateTime& dateTime) const;
    bool operator!=(const LocalDateTime& dateTime) const;
    bool operator<(const LocalDateTime& dateTime) const;
    bool operator<=(const LocalDateTime& dateTime) const;
    bool operator>(const LocalDateTime& dateTime) const;
    bool operator>=(const LocalDateTime& dateTime) const;

    LocalDateTime operator+(const Timespan& span) const;
    LocalDateTime operator-(const Timespan& span) const;
    Timespan operator-(const LocalDateTime& dateTime) const;
    LocalDateTime& operator+=(const Timespan& span);
    LocalDateTime& operator-=(const Timespan& span);

protected:
    LocalDateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff, int tzd);

    /// Recalculate the tzd based on the _dateTime member based
    /// on the current timezone using the Standard C runtime functions.
    /// If adjust is true, then adjustForTzd() is called after the
    /// differential is calculated.
    void determineTzd(bool adjust = false);

    /// Adjust the _dateTime member based on the _tzd member.
    void adjustForTzd();

    /// Determine the DST offset for the current date/time.
    std::time_t dstOffset(int& dstOffset) const;

private:
    DateTime _dateTime;
    int _tzd;

    friend class DateTimeFormatter;
    friend class DateTimeParser;
};


//
// LocalDateTime inlines
//


inline int LocalDateTime::year() const
{
    return _dateTime.year();
}


inline int LocalDateTime::month() const
{
    return _dateTime.month();
}


inline int LocalDateTime::week(int firstDayOfWeek) const
{
    return _dateTime.week(firstDayOfWeek);
}


inline int LocalDateTime::day() const
{
    return _dateTime.day();
}


inline int LocalDateTime::dayOfWeek() const
{
    return _dateTime.dayOfWeek();
}


inline int LocalDateTime::dayOfYear() const
{
    return _dateTime.dayOfYear();
}


inline int LocalDateTime::hour() const
{
    return _dateTime.hour();
}


inline int LocalDateTime::hourAMPM() const
{
    return _dateTime.hourAMPM();
}


inline bool LocalDateTime::isAM() const
{
    return _dateTime.isAM();
}


inline bool LocalDateTime::isPM() const
{
    return _dateTime.isPM();
}


inline int LocalDateTime::minute() const
{
    return _dateTime.minute();
}


inline int LocalDateTime::second() const
{
    return _dateTime.second();
}


inline int LocalDateTime::millisecond() const
{
    return _dateTime.millisecond();
}


inline int LocalDateTime::microsecond() const
{
    return _dateTime.microsecond();
}


inline double LocalDateTime::julianDay() const
{
    return _dateTime.julianDay();
}


inline int LocalDateTime::tzd() const
{
    return _tzd;
}


inline Timestamp LocalDateTime::timestamp() const
{
    return Timestamp::fromUtcTime(_dateTime.utcTime());
}


inline Timestamp::UtcTimeVal LocalDateTime::utcTime() const
{
    return _dateTime.utcTime() - ((Timestamp::TimeDiff)_tzd) * 10000000;
}


inline void LocalDateTime::adjustForTzd()
{
    _dateTime += Timespan(((Timestamp::TimeDiff)_tzd) * Timespan::SECONDS);
}


/// Exchange two LocalDateTime values.
inline void swap(LocalDateTime& d1, LocalDateTime& d2)
{
    d1.swap(d2);
}


// ---------------------------------------------------------------------
//

/// Definition of date/time formats and various
/// constants used by DateTimeFormatter and DateTimeParser.
class Base_API DateTimeFormat
{
public:
    /// predefined date formats
    /// The date/time format defined in the ISO 8601 standard.
    ///
    /// Examples:
    ///   2005-01-01T12:00:00+01:00
    ///   2005-01-01T11:00:00Z
    static const std::string ISO8601_FORMAT;

    /// The date/time format defined in the ISO 8601 standard,
    /// with fractional seconds.
    ///
    /// Examples:
    ///   2005-01-01T12:00:00.000000+01:00
    ///   2005-01-01T11:00:00.000000Z
    static const std::string ISO8601_FRAC_FORMAT;

    /// The date/time format defined in RFC 822 (obsoleted by RFC 1123).
    ///
    /// Examples:
    ///   Sat, 1 Jan 05 12:00:00 +0100
    ///   Sat, 1 Jan 05 11:00:00 GMT
    static const std::string RFC822_FORMAT;

    /// The date/time format defined in RFC 1123 (obsoletes RFC 822).
    ///
    /// Examples:
    ///   Sat, 1 Jan 2005 12:00:00 +0100
    ///   Sat, 1 Jan 2005 11:00:00 GMT
    static const std::string RFC1123_FORMAT;

    /// The date/time format defined in the HTTP specification (RFC 2616),
    /// which is basically a variant of RFC 1036 with a zero-padded day field.
    ///
    /// Examples:
    ///   Sat, 01 Jan 2005 12:00:00 +0100
    ///   Sat, 01 Jan 2005 11:00:00 GMT
    static const std::string HTTP_FORMAT;

    /// The date/time format defined in RFC 850 (obsoleted by RFC 1036).
    ///
    /// Examples:
    ///   Saturday, 1-Jan-05 12:00:00 +0100
    ///   Saturday, 1-Jan-05 11:00:00 GMT
    static const std::string RFC850_FORMAT;

    /// The date/time format defined in RFC 1036 (obsoletes RFC 850).
    ///
    /// Examples:
    ///   Saturday, 1 Jan 05 12:00:00 +0100
    ///   Saturday, 1 Jan 05 11:00:00 GMT
    static const std::string RFC1036_FORMAT;

    /// The date/time format produced by the ANSI C asctime() function.
    ///
    /// Example:
    ///   Sat Jan  1 12:00:00 2005
    static const std::string ASCTIME_FORMAT;

    /// A simple, sortable date/time format.
    ///
    /// Example:
    ///   2005-01-01 12:00:00
    static const std::string SORTABLE_FORMAT;

    /// names used by formatter and parser
    /// English names of week days (Sunday, Monday, Tuesday, ...).
    static const std::string WEEKDAY_NAMES[7];

    /// English names of months (January, February, ...).
    static const std::string MONTH_NAMES[12];
};


// ---------------------------------------------------------------------
//

/// This class converts dates and times into strings, supporting a
/// variety of standard and custom formats.
///
/// There are two kind of static member functions:
///    * format* functions
/// return a std::string containin the formatted value.
///    * append* functions append the formatted value to
///      an existing string.
class Base_API DateTimeFormatter
{
public:
    enum
    {
        UTC = 0xFFFF ///< Special value for timeZoneDifferential denoting UTC.
    };

    /// Formats the given timestamp according to the given format.
    /// The format string is used as a template to format the date and
    /// is copied character by character except for the following special
    /// characters, which are replaced by the corresponding value.
    ///
    ///   * %w - abbreviated weekday (Mon, Tue, ...)
    ///   * %W - full weekday (Monday, Tuesday, ...)
    ///   * %b - abbreviated month (Jan, Feb, ...)
    ///   * %B - full month (January, February, ...)
    ///   * %d - zero-padded day of month (01 .. 31)
    ///   * %e - day of month (1 .. 31)
    ///   * %f - space-padded day of month ( 1 .. 31)
    ///   * %m - zero-padded month (01 .. 12)
    ///   * %n - month (1 .. 12)
    ///   * %o - space-padded month ( 1 .. 12)
    ///   * %y - year without century (70)
    ///   * %Y - year with century (1970)
    ///   * %H - hour (00 .. 23)
    ///   * %h - hour (00 .. 12)
    ///   * %a - am/pm
    ///   * %A - AM/PM
    ///   * %M - minute (00 .. 59)
    ///   * %S - second (00 .. 59)
    ///   * %s - seconds and microseconds (equivalent to %S.%F)
    ///   * %i - millisecond (000 .. 999)
    ///   * %c - centisecond (0 .. 9)
    ///   * %F - fractional seconds/microseconds (000000 - 999999)
    ///   * %z - time zone differential in ISO 8601 format (Z or +NN.NN)
    ///   * %Z - time zone differential in RFC format (GMT or +NNNN)
    ///   * %% - percent sign
    ///
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static std::string format(const Timestamp& timestamp,
                              std::string_view fmt,
                              int timeZoneDifferential = UTC);

    /// Formats the given date and time according to the given format.
    /// See format(const Timestamp&, std::string_view, int) for more
    /// information.
    static std::string format(const DateTime& dateTime, std::string_view fmt,
                              int timeZoneDifferential = UTC);

    /// Formats the given local date and time according to the given format.
    /// See format(const Timestamp&, std::string_view, int) for more
    /// information.
    static std::string format(const LocalDateTime& dateTime,
                              std::string_view fmt);

    /// Formats the given timespan according to the given format.
    /// The format string is used as a template to format the date and
    /// is copied character by character except for the following special
    /// characters, which are replaced by the corresponding value.
    ///
    ///   * %d - days
    ///   * %H - hours     (00 .. 23)
    ///   * %h - total hours (0 .. n)
    ///   * %M - minutes (00 .. 59)
    ///   * %m - total minutes (0 .. n)
    ///   * %S - seconds (00 .. 59)
    ///   * %s - total seconds (0 .. n)
    ///   * %i - milliseconds (000 .. 999)
    ///   * %c - centisecond (0 .. 9)
    ///   * %F - fractional seconds/microseconds (000000 - 999999)
    ///   * %% - percent sign
    static std::string format(const Timespan& timespan,
                              std::string_view fmt = "%dd %H:%M:%S.%i");

    /// Formats the given timestamp according to the given format and appends it
    /// to str.
    ///
    /// See format() for documentation of the formatting string.
    static void append(std::string& str, const Timestamp& timestamp,
                       std::string_view fmt, int timeZoneDifferential = UTC);

    /// Formats the given date and time according to the given format and
    /// appends it to str.
    ///
    /// See format() for documentation of the formatting string.
    static void append(std::string& str, const DateTime& dateTime,
                       std::string_view fmt, int timeZoneDifferential = UTC);

    /// Formats the given local date and time according to the given format and
    /// appends it to str.
    ///
    /// See format() for documentation of the formatting string.
    static void append(std::string& str, const LocalDateTime& dateTime,
                       std::string_view fmt);

    /// Formats the given timespan according to the given format and appends it
    /// to str.
    ///
    /// See format() for documentation of the formatting string.
    static void append(std::string& str, const Timespan& timespan,
                       std::string_view fmt = "%dd %H:%M:%S.%i");

    /// Formats the given timezone differential in ISO format.
    /// If timeZoneDifferential is UTC, "Z" is returned,
    /// otherwise, +HH.MM (or -HH.MM) is returned.
    static std::string tzdISO(int timeZoneDifferential);

    /// Formats the given timezone differential in RFC format.
    /// If timeZoneDifferential is UTC, "GMT" is returned,
    /// otherwise ++HHMM (or -HHMM) is returned.
    static std::string tzdRFC(int timeZoneDifferential);

    /// Formats the given timezone differential in ISO format
    /// and appends it to the given string.
    /// If timeZoneDifferential is UTC, "Z" is returned,
    /// otherwise, +HH.MM (or -HH.MM) is returned.
    static void tzdISO(std::string& str, int timeZoneDifferential);

    /// Formats the given timezone differential in RFC format
    /// and appends it to the given string.
    /// If timeZoneDifferential is UTC, "GMT" is returned,
    /// otherwise ++HHMM (or -HHMM) is returned.
    static void tzdRFC(std::string& str, int timeZoneDifferential);
};


//
// DateTimeParser
//


/// This class provides a method for parsing dates and times
/// from strings. All parsing methods do their best to
/// parse a meaningful result, even from malformed input
/// strings.
///
/// The returned DateTime will always contain a time in the same
/// timezone as the time in the string. Call DateTime::makeUTC()
/// with the timeZoneDifferential returned by parse() to convert
/// the DateTime to UTC.
///
/// Note: When parsing a time in 12-hour (AM/PM) format, the hour
/// (%h) must be parsed before the AM/PM designator (%a, %A),
/// otherwise the AM/PM designator will be ignored.
///
/// See the DateTimeFormatter class for a list of supported format specifiers.
/// In addition to the format specifiers supported by DateTimeFormatter, an
/// additional specifier is supported: %r will parse a year given by either
/// two or four digits. Years 69-00 are interpreted in the 20th century
/// (1969-2000), years 01-68 in the 21th century (2001-2068).
class Base_API DateTimeParser
{
public:
    /// Parses a date and time in the given format from the given string.
    /// Throws a SyntaxException if the string cannot be successfully parsed.
    /// Please see DateTimeFormatter::format() for a description of the format
    /// string.
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static void parse(std::string_view fmt, std::string_view str,
                      DateTime& dateTime, int& timeZoneDifferential);

    /// Parses a date and time in the given format from the given string.
    /// Throws a SyntaxException if the string cannot be successfully parsed.
    /// Please see DateTimeFormatter::format() for a description of the format
    /// string.
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static DateTime parse(std::string_view fmt, std::string_view str,
                          int& timeZoneDifferential);

    /// Parses a date and time in the given format from the given string.
    /// Returns true if the string has been successfully parsed, false
    /// otherwise.
    /// Please see DateTimeFormatter::format() for a description of the format
    /// string.
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static bool tryParse(std::string_view fmt, std::string_view str,
                         DateTime& dateTime, int& timeZoneDifferential);

    /// Parses a date and time from the given dateTime string. Before parsing,
    /// the method
    /// examines the dateTime string for a known date/time format.
    /// Throws a SyntaxException if the string cannot be successfully parsed.
    /// Please see DateTimeFormatter::format() for a description of the format
    /// string.
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static void parse(std::string_view str, DateTime& dateTime,
                      int& timeZoneDifferential);

    /// Parses a date and time from the given dateTime string. Before parsing,
    /// the method
    /// examines the dateTime string for a known date/time format.
    /// Please see DateTimeFormatter::format() for a description of the format
    /// string.
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static DateTime parse(std::string_view str, int& timeZoneDifferential);

    /// Parses a date and time from the given dateTime string. Before parsing,
    /// the method
    /// examines the dateTime string for a known date/time format.
    /// Please see DateTimeFormatter::format() for a description of the format
    /// string.
    /// Class DateTimeFormat defines format strings for various standard
    /// date/time formats.
    static bool tryParse(std::string_view str, DateTime& dateTime,
                         int& timeZoneDifferential);

    /// Tries to interpret the given range as a month name. The range must be at
    /// least
    /// three characters long.
    /// Returns the month number (1 .. 12) if the month name is valid. Otherwise
    /// throws
    /// a SyntaxException.
    static int parseMonth(const char*& it, const char* end);

    /// Tries to interpret the given range as a weekday name. The range must be
    /// at least
    /// three characters long.
    /// Returns the weekday number (0 .. 6, where 0 = Synday, 1 = Monday, etc.)
    /// if the
    /// weekday name is valid. Otherwise throws a SyntaxException.
    static int parseDayOfWeek(const char*& it, const char* end);


protected:
    static int parseTZD(const char*& it, const char* end);
    static int parseAMPM(const char*& it, const char* end, int hour);
};


//
// Timestamp inlines
//


inline bool Timestamp::operator==(const Timestamp& ts) const
{
    return _ts == ts._ts;
}


inline bool Timestamp::operator!=(const Timestamp& ts) const
{
    return _ts != ts._ts;
}


inline bool Timestamp::operator>(const Timestamp& ts) const
{
    return _ts > ts._ts;
}


inline bool Timestamp::operator>=(const Timestamp& ts) const
{
    return _ts >= ts._ts;
}


inline bool Timestamp::operator<(const Timestamp& ts) const
{
    return _ts < ts._ts;
}


inline bool Timestamp::operator<=(const Timestamp& ts) const
{
    return _ts <= ts._ts;
}


inline Timestamp Timestamp::operator+(Timestamp::TimeDiff d) const
{
    return Timestamp(_ts + d);
}


inline Timestamp Timestamp::operator-(Timestamp::TimeDiff d) const
{
    return Timestamp(_ts - d);
}


inline Timestamp::TimeDiff Timestamp::operator-(const Timestamp& ts) const
{
    return _ts - ts._ts;
}


inline Timestamp& Timestamp::operator+=(Timestamp::TimeDiff d)
{
    _ts += d;
    return *this;
}


inline Timestamp& Timestamp::operator-=(Timestamp::TimeDiff d)
{
    _ts -= d;
    return *this;
}


inline std::time_t Timestamp::epochTime() const
{
    return std::time_t(_ts / resolution());
}


inline Timestamp::UtcTimeVal Timestamp::utcTime() const
{
    return _ts * 10 + (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
}


inline Timestamp::TimeVal Timestamp::epochMicroseconds() const
{
    return _ts;
}


inline Timestamp::TimeDiff Timestamp::elapsed() const
{
    Timestamp now;
    return now - *this;
}


inline bool Timestamp::isElapsed(Timestamp::TimeDiff interval) const
{
    Timestamp now;
    Timestamp::TimeDiff diff = now - *this;
    return diff >= interval;
}


inline Timestamp::TimeVal Timestamp::resolution()
{
    return 1000000;
}


/// Exchange two Timestamp values.
inline void swap(Timestamp& s1, Timestamp& s2)
{
    s1.swap(s2);
}


//
// Timespan inlines
//


inline int Timespan::days() const
{
    return int(_span / DAYS);
}


inline int Timespan::hours() const
{
    return int((_span / HOURS) % 24);
}


inline int Timespan::totalHours() const
{
    return int(_span / HOURS);
}


inline int Timespan::minutes() const
{
    return int((_span / MINUTES) % 60);
}


inline int Timespan::totalMinutes() const
{
    return int(_span / MINUTES);
}


inline int Timespan::seconds() const
{
    return int((_span / SECONDS) % 60);
}


inline int Timespan::totalSeconds() const
{
    return int(_span / SECONDS);
}


inline int Timespan::milliseconds() const
{
    return int((_span / MILLISECONDS) % 1000);
}


inline Timespan::TimeDiff Timespan::totalMilliseconds() const
{
    return _span / MILLISECONDS;
}


inline int Timespan::microseconds() const
{
    return int(_span % 1000);
}


inline int Timespan::useconds() const
{
    return int(_span % 1000000);
}


inline Timespan::TimeDiff Timespan::totalMicroseconds() const
{
    return _span;
}


inline bool Timespan::operator==(const Timespan& ts) const
{
    return _span == ts._span;
}


inline bool Timespan::operator!=(const Timespan& ts) const
{
    return _span != ts._span;
}


inline bool Timespan::operator>(const Timespan& ts) const
{
    return _span > ts._span;
}


inline bool Timespan::operator>=(const Timespan& ts) const
{
    return _span >= ts._span;
}


inline bool Timespan::operator<(const Timespan& ts) const
{
    return _span < ts._span;
}


inline bool Timespan::operator<=(const Timespan& ts) const
{
    return _span <= ts._span;
}


inline bool Timespan::operator==(TimeDiff microseconds) const
{
    return _span == microseconds;
}


inline bool Timespan::operator!=(TimeDiff microseconds) const
{
    return _span != microseconds;
}


inline bool Timespan::operator>(TimeDiff microseconds) const
{
    return _span > microseconds;
}


inline bool Timespan::operator>=(TimeDiff microseconds) const
{
    return _span >= microseconds;
}


inline bool Timespan::operator<(TimeDiff microseconds) const
{
    return _span < microseconds;
}


inline bool Timespan::operator<=(TimeDiff microseconds) const
{
    return _span <= microseconds;
}


/// Exchange two Timespan values.
inline void swap(Timespan& s1, Timespan& s2)
{
    s1.swap(s2);
}


//
// DateTimeFormatter inlines
//


inline std::string DateTimeFormatter::format(const Timestamp& timestamp,
                                             std::string_view fmt,
                                             int timeZoneDifferential)
{
    DateTime dateTime(timestamp);
    return format(dateTime, fmt, timeZoneDifferential);
}


inline std::string DateTimeFormatter::format(const DateTime& dateTime,
                                             std::string_view fmt,
                                             int timeZoneDifferential)
{
    std::string result;
    result.reserve(64);
    append(result, dateTime, fmt, timeZoneDifferential);
    return result;
}


inline std::string DateTimeFormatter::format(const LocalDateTime& dateTime,
                                             std::string_view fmt)
{
    return format(dateTime._dateTime, fmt, dateTime._tzd);
}


inline std::string DateTimeFormatter::format(const Timespan& timespan,
                                             std::string_view fmt)
{
    std::string result;
    result.reserve(32);
    append(result, timespan, fmt);
    return result;
}


inline void DateTimeFormatter::append(std::string& str,
                                      const Timestamp& timestamp,
                                      std::string_view fmt,
                                      int timeZoneDifferential)
{
    DateTime dateTime(timestamp);
    append(str, dateTime, fmt, timeZoneDifferential);
}


inline std::string DateTimeFormatter::tzdISO(int timeZoneDifferential)
{
    std::string result;
    result.reserve(8);
    tzdISO(result, timeZoneDifferential);
    return result;
}


inline std::string DateTimeFormatter::tzdRFC(int timeZoneDifferential)
{
    std::string result;
    result.reserve(8);
    tzdRFC(result, timeZoneDifferential);
    return result;
}


//
// Stopwatch
//


/// A simple facility to measure time intervals
/// with microsecond resolution.
///
/// The Stopwatch uses the current system time, so if the
/// system time changes the measured time will be incorrect.
class Base_API Stopwatch
{
public:
    Stopwatch();
    ~Stopwatch();

    /// Starts (or restarts) the stopwatch.
    void start();

    /// Stops or pauses the stopwatch.
    void stop();

    /// Resets the stopwatch.
    void reset();

    /// Resets and starts the stopwatch.
    void restart();

    /// Returns the elapsed time in microseconds
    /// since the stopwatch started.
    Timestamp::TimeDiff elapsed() const;

    /// Returns the number of seconds elapsed
    /// since the stopwatch started.
    int elapsedSeconds() const;

    /// Returns the number of milliseconds elapsed
    /// since the stopwatch started.
    int elapsedMilliseconds() const;

    /// Returns the resolution of the stopwatch.
    static Timestamp::TimeVal resolution();

private:
    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;

    Timestamp _start;
    Timestamp::TimeDiff _elapsed;
    bool _running;
};


} // namespace icy


/// @}
