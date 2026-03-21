///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>


namespace icy {


/// Classes and functions for handling time
namespace time {


/// Constants for calculating time.
static const int64_t kNumMillisecsPerSec = INT64_C(1000);
static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

static const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMillisec = kNumNanosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMicrosec = kNumNanosecsPerSec / kNumMicrosecsPerSec;

/// The date/time format defined in the ISO 8601 standard.
/// This is the default format used throughout the library for consistency.
///
/// Examples:
///   2005-01-01T12:00:00+01:00
///   2005-01-01T11:00:00Z
static const char* ISO8601Format = "%Y-%m-%dT%H:%M:%SZ";

/// Returns the current wall-clock time as a UTC time_t (seconds since epoch).
/// @return Current UTC time in seconds since the Unix epoch.
Base_API std::time_t now();

/// Returns the elapsed process time in decimal seconds using a monotonic clock.
/// @return Process time in seconds.
Base_API double clockSecs();

/// Formats a broken-down time value using the given strftime format string.
/// @param dt  Broken-down time to format.
/// @param fmt strftime format string (default: ISO8601Format).
/// @return Formatted time string.
Base_API std::string print(const std::tm& dt, const char* fmt = ISO8601Format);

/// Formats the current local time using the given strftime format string.
/// @param fmt strftime format string (default: ISO8601Format).
/// @return Formatted local time string.
Base_API std::string printLocal(const char* fmt = ISO8601Format);

/// Formats the current UTC time using the given strftime format string.
/// @param fmt strftime format string (default: ISO8601Format).
/// @return Formatted UTC time string.
Base_API std::string printUTC(const char* fmt = ISO8601Format);

/// Converts a time_t value to a broken-down local time structure.
/// Uses thread-safe native functions (localtime_r / localtime_s).
/// @param time UTC time value to convert.
/// @return Broken-down local time.
Base_API std::tm toLocal(const std::time_t& time);

/// Converts a time_t value to a broken-down UTC time structure.
/// Uses thread-safe native functions (gmtime_r / gmtime_s).
/// @param time UTC time value to convert.
/// @return Broken-down UTC time.
Base_API std::tm toUTC(const std::time_t& time);

/// Returns the current local time as an ISO8601 formatted string.
/// @return ISO8601 local time string.
Base_API std::string getLocal();

/// Returns the current UTC time as an ISO8601 formatted string.
/// @return ISO8601 UTC time string.
Base_API std::string getUTC();

/// Returns the current high-resolution monotonic time in nanoseconds.
/// @return Current time in nanoseconds (suitable for measuring intervals).
Base_API uint64_t hrtime();


} // namespace time
} // namespace icy


/// @\}
