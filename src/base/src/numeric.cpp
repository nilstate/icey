///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/numeric.h"

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <stdexcept>


namespace icy {
namespace numeric {


namespace {


// Append the decimal representation of value to str.
template <typename T>
void appendDec(std::string& str, T value)
{
    char buffer[24];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
    str.append(buffer, ptr);
}


// Append the decimal representation of value to str,
// right-justified in a field of at least `width` characters (space-padded).
template <typename T>
void appendDecWidth(std::string& str, T value, int width)
{
    char buffer[24];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
    int len = static_cast<int>(ptr - buffer);
    for (int i = len; i < width; ++i)
        str.push_back(' ');
    str.append(buffer, ptr);
}


// Append the decimal representation of value to str,
// right-justified and zero-padded in a field of at least `width` characters.
template <typename T>
void appendDec0(std::string& str, T value, int width)
{
    char buffer[24];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
    int len = static_cast<int>(ptr - buffer);

    // Handle negative: emit '-' then zero-pad the digits
    if constexpr (std::is_signed_v<T>) {
        if (value < 0 && len > 1) {
            str.push_back('-');
            // Skip the '-' in buffer for padding
            for (int i = len - 1; i < width; ++i)
                str.push_back('0');
            str.append(buffer + 1, ptr);
            return;
        }
    }

    for (int i = len; i < width; ++i)
        str.push_back('0');
    str.append(buffer, ptr);
}


// Append the uppercase hexadecimal representation of value to str.
template <typename T>
void appendHex(std::string& str, T value)
{
    using U = std::make_unsigned_t<T>;
    U uval = static_cast<U>(value);
    char buffer[24];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), uval, 16);
    // to_chars produces lowercase hex; convert to uppercase
    std::transform(buffer, ptr, buffer, [](char c) {
        return (c >= 'a' && c <= 'f') ? static_cast<char>(c - 'a' + 'A') : c;
    });
    str.append(buffer, ptr);
}


// Append the uppercase hexadecimal representation of value to str,
// right-justified and zero-padded in a field of at least `width` characters.
template <typename T>
void appendHex0(std::string& str, T value, int width)
{
    using U = std::make_unsigned_t<T>;
    U uval = static_cast<U>(value);
    char buffer[24];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), uval, 16);
    std::transform(buffer, ptr, buffer, [](char c) {
        return (c >= 'a' && c <= 'f') ? static_cast<char>(c - 'a' + 'A') : c;
    });
    int len = static_cast<int>(ptr - buffer);
    for (int i = len; i < width; ++i)
        str.push_back('0');
    str.append(buffer, ptr);
}


inline void checkWidth(int width)
{
    if (width <= 0 || width >= 64)
        throw std::invalid_argument("Numeric: width out of range");
}


} // anonymous namespace


//
// int
//

void format(std::string& str, int value) { appendDec(str, value); }
void format(std::string& str, int value, int width) { checkWidth(width); appendDecWidth(str, value, width); }
void format0(std::string& str, int value, int width) { checkWidth(width); appendDec0(str, value, width); }
void formatHex(std::string& str, int value) { appendHex(str, value); }
void formatHex(std::string& str, int value, int width) { checkWidth(width); appendHex0(str, value, width); }


//
// unsigned int
//

void format(std::string& str, unsigned value) { appendDec(str, value); }
void format(std::string& str, unsigned value, int width) { checkWidth(width); appendDecWidth(str, value, width); }
void format0(std::string& str, unsigned int value, int width) { checkWidth(width); appendDec0(str, value, width); }
void formatHex(std::string& str, unsigned value) { appendHex(str, value); }
void formatHex(std::string& str, unsigned value, int width) { checkWidth(width); appendHex0(str, value, width); }


//
// long
//

void format(std::string& str, long value) { appendDec(str, value); }
void format(std::string& str, long value, int width) { checkWidth(width); appendDecWidth(str, value, width); }
void format0(std::string& str, long value, int width) { checkWidth(width); appendDec0(str, value, width); }
void formatHex(std::string& str, long value) { appendHex(str, value); }
void formatHex(std::string& str, long value, int width) { checkWidth(width); appendHex0(str, value, width); }


//
// unsigned long
//

void format(std::string& str, unsigned long value) { appendDec(str, value); }
void format(std::string& str, unsigned long value, int width) { checkWidth(width); appendDecWidth(str, value, width); }
void format0(std::string& str, unsigned long value, int width) { checkWidth(width); appendDec0(str, value, width); }
void formatHex(std::string& str, unsigned long value) { appendHex(str, value); }
void formatHex(std::string& str, unsigned long value, int width) { checkWidth(width); appendHex0(str, value, width); }


//
// int64_t / uint64_t
//
// On LP64 platforms (Linux, macOS), long is 64-bit and these are the same type.
// On LLP64 platforms (Windows), long is 32-bit and these are distinct.
// Only define if int64_t is not the same type as long.
//

#if !defined(__LP64__) && !defined(__APPLE__) && !(defined(__SIZEOF_LONG__) && __SIZEOF_LONG__ == 8)

void format(std::string& str, std::int64_t value) { appendDec(str, value); }
void format(std::string& str, std::int64_t value, int width) { checkWidth(width); appendDecWidth(str, value, width); }
void format0(std::string& str, std::int64_t value, int width) { checkWidth(width); appendDec0(str, value, width); }
void formatHex(std::string& str, std::int64_t value) { appendHex(str, value); }
void formatHex(std::string& str, std::int64_t value, int width) { checkWidth(width); appendHex0(str, value, width); }

void format(std::string& str, uint64_t value) { appendDec(str, value); }
void format(std::string& str, uint64_t value, int width) { checkWidth(width); appendDecWidth(str, value, width); }
void format0(std::string& str, uint64_t value, int width) { checkWidth(width); appendDec0(str, value, width); }
void formatHex(std::string& str, uint64_t value) { appendHex(str, value); }
void formatHex(std::string& str, uint64_t value, int width) { checkWidth(width); appendHex0(str, value, width); }

#endif


//
// void*
//

void format(std::string& str, const void* ptr)
{
    char buffer[24];
    std::snprintf(buffer, sizeof(buffer), "%p", ptr);
    str.append(buffer);
}


} // namespace numeric
} // namespace icy


/// @\}
