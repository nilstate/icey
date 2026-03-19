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

/// Return the number of UTC milliseconds since epoch.
Base_API std::time_t now();

/// Return the current process time in decimal seconds.
Base_API double clockSecs();

/// Cross-platform time formatting.
Base_API std::string print(const std::tm& dt, const char* fmt = ISO8601Format);

/// Print the current local time using the given format.
Base_API std::string printLocal(const char* fmt = ISO8601Format);

/// Print the current UTC time using the given format.
Base_API std::string printUTC(const char* fmt = ISO8601Format);

/// Convert the given time value to local time.
/// Uses thread-safe native functions.
Base_API std::tm toLocal(const std::time_t& time);

/// Convert the given time value to UTC time.
/// Uses thread-safe native functions.
Base_API std::tm toUTC(const std::time_t& time);

/// Return a local ISO8601 formatted date time string.
Base_API std::string getLocal();

/// Return a UTC ISO8601 formatted date time string.
Base_API std::string getUTC();

/// Return the current high-resolution real time in nanoseconds.
Base_API uint64_t hrtime();


} // namespace time
} // namespace icy


/// @\}
