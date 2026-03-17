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


#pragma once


#include <cstdint>
#include <string>


namespace scy {
namespace numeric {


//
// Integer To String Formatting
//


/// Formats an integer value in decimal notation.
void format(std::string& str, int value);

/// Formats an integer value in decimal notation,
/// right justified in a field having at least
/// the specified width.
void format(std::string& str, int value, int width);

/// Formats an integer value in decimal notation,
/// right justified and zero-padded in a field
/// having at least the specified width.
void format0(std::string& str, int value, int width);

/// Formats an int value in hexadecimal notation.
/// The value is treated as unsigned.
void formatHex(std::string& str, int value);

/// Formats a int value in hexadecimal notation,
/// right justified and zero-padded in
/// a field having at least the specified width.
/// The value is treated as unsigned.
void formatHex(std::string& str, int value, int width);

/// Formats an unsigned int value in decimal notation.
void format(std::string& str, unsigned value);

/// Formats an unsigned long int in decimal notation,
/// right justified in a field having at least the
/// specified width.
void format(std::string& str, unsigned value, int width);

/// Formats an unsigned int value in decimal notation,
/// right justified and zero-padded in a field having at
/// least the specified width.
void format0(std::string& str, unsigned int value, int width);

/// Formats an unsigned int value in hexadecimal notation.
void formatHex(std::string& str, unsigned value);

/// Formats a int value in hexadecimal notation,
/// right justified and zero-padded in
/// a field having at least the specified width.
void formatHex(std::string& str, unsigned value, int width);

/// Formats a long value in decimal notation.
void format(std::string& str, long value);

/// Formats a long value in decimal notation,
/// right justified in a field having at least the
/// specified width.
void format(std::string& str, long value, int width);

/// Formats a long value in decimal notation,
/// right justified and zero-padded in a field
/// having at least the specified width.
void format0(std::string& str, long value, int width);

/// Formats an unsigned long value in hexadecimal notation.
/// The value is treated as unsigned.
void formatHex(std::string& str, long value);

/// Formats an unsigned long value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the
/// specified width.
/// The value is treated as unsigned.
void formatHex(std::string& str, long value, int width);

/// Formats an unsigned long value in decimal notation.
void format(std::string& str, unsigned long value);

/// Formats an unsigned long value in decimal notation,
/// right justified in a field having at least the specified
/// width.
void format(std::string& str, unsigned long value, int width);

/// Formats an unsigned long value in decimal notation,
/// right justified and zero-padded
/// in a field having at least the specified width.
void format0(std::string& str, unsigned long value, int width);

/// Formats an unsigned long value in hexadecimal notation.
void formatHex(std::string& str, unsigned long value);

/// Formats an unsigned long value in hexadecimal notation,
/// right justified and zero-padded in a field having at least the
/// specified width.
void formatHex(std::string& str, unsigned long value, int width);

/// Formats a 64-bit integer value in decimal notation.
void format(std::string& str, std::int64_t value);

/// Formats a 64-bit integer value in decimal notation,
/// right justified in a field having at least the specified width.
void format(std::string& str, std::int64_t value, int width);

/// Formats a 64-bit integer value in decimal notation,
/// right justified and zero-padded in a field having at least
/// the specified width.
void format0(std::string& str, std::int64_t value, int width);

/// Formats a 64-bit integer value in hexadecimal notation.
/// The value is treated as unsigned.
void formatHex(std::string& str, std::int64_t value);

/// Formats a 64-bit integer value in hexadecimal notation,
/// right justified and zero-padded in a field having at least
/// the specified width.
/// The value is treated as unsigned.
void formatHex(std::string& str, std::int64_t value, int width);

/// Formats an unsigned 64-bit integer value in decimal notation.
void format(std::string& str, uint64_t value);

/// Formats an unsigned 64-bit integer value in decimal notation,
/// right justified in a field having at least the specified width.
void format(std::string& str, uint64_t value, int width);

/// Formats an unsigned 64-bit integer value in decimal notation,
/// right justified and zero-padded in a field having at least the
/// specified width.
void format0(std::string& str, uint64_t value, int width);

/// Formats a 64-bit integer value in hexadecimal notation.
void formatHex(std::string& str, uint64_t value);

/// Formats a 64-bit integer value in hexadecimal notation,
/// right justified and zero-padded in a field having at least
/// the specified width.
void formatHex(std::string& str, uint64_t value, int width);


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
