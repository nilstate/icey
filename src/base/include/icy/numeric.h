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


#include <cstdint>
#include <string>


namespace icy {
/// Integer parsing and formatting helpers.
namespace numeric {


//
// Integer To String Formatting
//


/// Formats an integer value in decimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void format(std::string& str, int value);

/// Formats an integer value in decimal notation,
/// right justified in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with spaces on the left.
void format(std::string& str, int value, int width);

/// Formats an integer value in decimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void format0(std::string& str, int value, int width);

/// Formats an int value in hexadecimal notation.
/// The value is treated as unsigned.
/// @param str   Output string to append to.
/// @param value Value to format.
void formatHex(std::string& str, int value);

/// Formats an int value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// The value is treated as unsigned.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void formatHex(std::string& str, int value, int width);

/// Formats an unsigned int value in decimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void format(std::string& str, unsigned value);

/// Formats an unsigned int value in decimal notation,
/// right justified in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with spaces on the left.
void format(std::string& str, unsigned value, int width);

/// Formats an unsigned int value in decimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void format0(std::string& str, unsigned int value, int width);

/// Formats an unsigned int value in hexadecimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void formatHex(std::string& str, unsigned value);

/// Formats an unsigned int value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void formatHex(std::string& str, unsigned value, int width);

/// Formats a long value in decimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void format(std::string& str, long value);

/// Formats a long value in decimal notation,
/// right justified in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with spaces on the left.
void format(std::string& str, long value, int width);

/// Formats a long value in decimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void format0(std::string& str, long value, int width);

/// Formats a long value in hexadecimal notation.
/// The value is treated as unsigned.
/// @param str   Output string to append to.
/// @param value Value to format.
void formatHex(std::string& str, long value);

/// Formats a long value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// The value is treated as unsigned.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void formatHex(std::string& str, long value, int width);

/// Formats an unsigned long value in decimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void format(std::string& str, unsigned long value);

/// Formats an unsigned long value in decimal notation,
/// right justified in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with spaces on the left.
void format(std::string& str, unsigned long value, int width);

/// Formats an unsigned long value in decimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void format0(std::string& str, unsigned long value, int width);

/// Formats an unsigned long value in hexadecimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void formatHex(std::string& str, unsigned long value);

/// Formats an unsigned long value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void formatHex(std::string& str, unsigned long value, int width);

/// Formats a 64-bit integer value in decimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void format(std::string& str, std::int64_t value);

/// Formats a 64-bit integer value in decimal notation,
/// right justified in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with spaces on the left.
void format(std::string& str, std::int64_t value, int width);

/// Formats a 64-bit integer value in decimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void format0(std::string& str, std::int64_t value, int width);

/// Formats a 64-bit integer value in hexadecimal notation.
/// The value is treated as unsigned.
/// @param str   Output string to append to.
/// @param value Value to format.
void formatHex(std::string& str, std::int64_t value);

/// Formats a 64-bit integer value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// The value is treated as unsigned.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void formatHex(std::string& str, std::int64_t value, int width);

/// Formats an unsigned 64-bit integer value in decimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void format(std::string& str, uint64_t value);

/// Formats an unsigned 64-bit integer value in decimal notation,
/// right justified in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with spaces on the left.
void format(std::string& str, uint64_t value, int width);

/// Formats an unsigned 64-bit integer value in decimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void format0(std::string& str, uint64_t value, int width);

/// Formats an unsigned 64-bit integer value in hexadecimal notation.
/// @param str   Output string to append to.
/// @param value Value to format.
void formatHex(std::string& str, uint64_t value);

/// Formats an unsigned 64-bit integer value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the specified width.
/// @param str   Output string to append to.
/// @param value Value to format.
/// @param width Minimum field width; padded with zeros on the left.
void formatHex(std::string& str, uint64_t value, int width);


} // namespace numeric
} // namespace icy


/// @}
