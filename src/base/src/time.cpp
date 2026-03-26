///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/time.h"

#include "uv.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <locale>
#include <sstream>


namespace icy {
namespace time {


std::time_t now()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}


double clockSecs()
{
    using namespace std::chrono;
    auto t = steady_clock::now().time_since_epoch();
    return duration_cast<duration<double>>(t).count();
}


std::tm toLocal(const std::time_t& time)
{
    std::tm tm_snapshot;
#if defined(_WIN32)
    localtime_s(&tm_snapshot, &time); // thread-safe?
#else
    localtime_r(&time, &tm_snapshot); // POSIX
#endif
    return tm_snapshot;
}


std::tm toUTC(const std::time_t& time)
{
    std::tm tm_snapshot;
#if defined(_WIN32)
    gmtime_s(&tm_snapshot, &time); // thread-safe?
#else
    gmtime_r(&time, &tm_snapshot); // POSIX
#endif
    return tm_snapshot;
}


std::string print(const std::tm& dt, const char* fmt)
{
    std::ostringstream oss;
    oss << std::put_time(&dt, fmt);
    return oss.str();
}


std::string printLocal(const char* fmt)
{
    return print(toLocal(now()), fmt);
}


std::string printUTC(const char* fmt)
{
    return print(toUTC(now()), fmt);
}


std::string getLocal()
{
    return printLocal(ISO8601Format);
}


std::string getUTC()
{
    return printUTC(ISO8601Format);
}


uint64_t hrtime()
{
    return uv_hrtime();
}


} // namespace time
} // namespace icy


/// @}
