///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{
//
// This file uses functions from POCO C++ Libraries (license below)
//


#include "scy/numeric.h"

#include <stdexcept>


#if defined(_MSC_VER) || defined(__MINGW32__)
#define I64_FMT "I64"
#elif defined(__APPLE__)
#define I64_FMT "q"
#else
#define I64_FMT "ll"
#endif


namespace scy {
namespace numeric {


void format(std::string& str, int value)
{
    char buffer[64];
    std::sprintf(buffer, "%d", value);
    str.append(buffer);
}


void format(std::string& str, int value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%*d", width, value);
    str.append(buffer);
}


void format0(std::string& str, int value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*d", width, value);
    str.append(buffer);
}


void formatHex(std::string& str, int value)
{
    char buffer[64];
    std::sprintf(buffer, "%X", value);
    str.append(buffer);
}


void formatHex(std::string& str, int value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*X", width, value);
    str.append(buffer);
}


void format(std::string& str, unsigned value)
{
    char buffer[64];
    std::sprintf(buffer, "%u", value);
    str.append(buffer);
}


void format(std::string& str, unsigned value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%*u", width, value);
    str.append(buffer);
}


void format0(std::string& str, unsigned int value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*u", width, value);
    str.append(buffer);
}


void formatHex(std::string& str, unsigned value)
{
    char buffer[64];
    std::sprintf(buffer, "%X", value);
    str.append(buffer);
}


void formatHex(std::string& str, unsigned value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*X", width, value);
    str.append(buffer);
}


void format(std::string& str, long value)
{
    char buffer[64];
    std::sprintf(buffer, "%ld", value);
    str.append(buffer);
}


void format(std::string& str, long value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%*ld", width, value);
    str.append(buffer);
}


void format0(std::string& str, long value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*ld", width, value);
    str.append(buffer);
}


void formatHex(std::string& str, long value)
{
    char buffer[64];
    std::sprintf(buffer, "%lX", value);
    str.append(buffer);
}


void formatHex(std::string& str, long value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*lX", width, value);
    str.append(buffer);
}


void format(std::string& str, unsigned long value)
{
    char buffer[64];
    std::sprintf(buffer, "%lu", value);
    str.append(buffer);
}


void format(std::string& str, unsigned long value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%*lu", width, value);
    str.append(buffer);
}


void format0(std::string& str, unsigned long value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*lu", width, value);
    str.append(buffer);
}


void formatHex(std::string& str, unsigned long value)
{
    char buffer[64];
    std::sprintf(buffer, "%lX", value);
    str.append(buffer);
}


void formatHex(std::string& str, unsigned long value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*lX", width, value);
    str.append(buffer);
}


#if defined(SCY_HAVE_INT64) && !defined(SCY_LONG_IS_64_BIT)


void format(std::string& str, std::int64_t value)
{
    char buffer[64];
    std::sprintf(buffer, "%" I64_FMT "d", value);
    str.append(buffer);
}


void format(std::string& str, std::int64_t value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%*" I64_FMT "d", width, value);
    str.append(buffer);
}


void format0(std::string& str, std::int64_t value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*" I64_FMT "d", width, value);
    str.append(buffer);
}


void formatHex(std::string& str, std::int64_t value)
{
    char buffer[64];
    std::sprintf(buffer, "%" I64_FMT "X", value);
    str.append(buffer);
}


void formatHex(std::string& str, std::int64_t value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*" I64_FMT "X", width, value);
    str.append(buffer);
}


void format(std::string& str, uint64_t value)
{
    char buffer[64];
    std::sprintf(buffer, "%" I64_FMT "u", value);
    str.append(buffer);
}


void format(std::string& str, uint64_t value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%*" I64_FMT "u", width, value);
    str.append(buffer);
}


void format0(std::string& str, uint64_t value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*" I64_FMT "u", width, value);
    str.append(buffer);
}


void formatHex(std::string& str, uint64_t value)
{
    char buffer[64];
    std::sprintf(buffer, "%" I64_FMT "X", value);
    str.append(buffer);
}


void formatHex(std::string& str, uint64_t value, int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");

    char buffer[64];
    std::sprintf(buffer, "%0*" I64_FMT "X", width, value);
    str.append(buffer);
}


#endif // defined(SCY_HAVE_INT64) && !defined(SCY_LONG_IS_64_BIT)


void format(std::string& str, const void* ptr)
{
    char buffer[24];
    std::sprintf(buffer, "%p", ptr);
    str.append(buffer);
}


} // namespace numeric
} // namespace scy


/// @\}


//
// Copyright (c) 2004-2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
