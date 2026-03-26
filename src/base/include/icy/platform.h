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

#include <cstdint>
#include <string>
#include <string_view>


namespace icy {


//
/// Cross-platform utilities
//

/// Returns the current executable path.
Base_API std::string getExePath();

/// Return the current working directory.
Base_API std::string getCwd();

/// Returns the current amount of free memory.
Base_API uint64_t getFreeMemory();

/// Returns the current amount of used memory.
Base_API uint64_t getTotalMemory();

/// Returns the number of CPU cores.
Base_API int numCpuCores();

/// Pause the current thread for the given ms duration.
Base_API void sleep(int ms);

/// Pause the current thread until enter is pressed.
Base_API void pause();

/// Return the system hostname.
Base_API std::string getHostname();

/// Return an environment variable or the default value.
Base_API std::string getEnv(std::string_view name, std::string_view defaultValue = "");

/// Return an environment variable boolean or the default value.
/// The variable must be `1` or `true` for this function to return true.
Base_API bool getEnvBool(std::string_view name);


//
/// Windows helpers
//

#ifdef ICY_WIN

/// Returns true if the current OS is Windows Vista or later.
/// @return true if running on Windows Vista (6.0) or a newer release.
Base_API bool isWindowsVistaOrLater();

/// Returns true if the current OS is Windows XP or later.
/// @return true if running on Windows XP (5.1) or a newer release.
Base_API bool isWindowsXpOrLater();

/// Converts a UTF-8 byte sequence to a UTF-16 wide string.
/// @param utf8 Pointer to the UTF-8 encoded input.
/// @param len  Number of bytes to convert.
/// @return UTF-16 wide string.
Base_API std::wstring toUtf16(const char* utf8, size_t len);

/// Converts a UTF-8 string_view to a UTF-16 wide string.
/// @param str UTF-8 encoded input.
/// @return UTF-16 wide string.
Base_API std::wstring toUtf16(std::string_view str);

/// Converts a UTF-16 wide character sequence to a UTF-8 string.
/// @param wide Pointer to the UTF-16 encoded input.
/// @param len  Number of wide characters to convert.
/// @return UTF-8 encoded string.
Base_API std::string toUtf8(const wchar_t* wide, size_t len);

/// Converts a UTF-16 wide string to a UTF-8 string.
/// @param wstr UTF-16 encoded input.
/// @return UTF-8 encoded string.
Base_API std::string toUtf8(const std::wstring& wstr);

#endif


} // namespace icy

/// @}
