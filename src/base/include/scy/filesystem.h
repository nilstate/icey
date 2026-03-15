///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include <cstdint>
#include <string>
#include <vector>


namespace scy {
namespace fs {


/// The platform specific path separator string:
/// "/" on unix and "\\" on windows.
Base_API extern const char* separator;

/// The platform specific path separator character:
/// '/' on unix and '\\' on windows.
Base_API extern const char delimiter;

/// Returns the file name and extension part of the given path.
Base_API std::string filename(const std::string& path);

/// Returns the file name sans extension.
Base_API std::string basename(const std::string& path);

/// Returns the directory part of the path.
Base_API std::string dirname(const std::string& path);

/// Returns the file extension part of the path (including dot).
/// If `includeDot` is false, the leading dot is stripped.
Base_API std::string extname(const std::string& path, bool includeDot = true);

/// Returns true if the file or directory exists.
Base_API bool exists(const std::string& path);

/// Returns true if the path is a directory.
Base_API bool isdir(const std::string& path);

/// Returns the size in bytes of the given file, or -1 if file doesn't exist.
Base_API std::int64_t filesize(const std::string& path);

/// Returns a list of all files and folders in the directory.
Base_API void readdir(const std::string& path, std::vector<std::string>& res);

/// Creates a directory.
Base_API void mkdir(const std::string& path, int mode = 0755);

/// Creates a directory recursively.
Base_API void mkdirr(const std::string& path, int mode = 0755);

/// Removes a directory.
Base_API void rmdir(const std::string& path);

/// Deletes a file.
Base_API void unlink(const std::string& path);

/// Renames or moves the given file to the target path.
Base_API void rename(const std::string& path, const std::string& target);

/// Adds the trailing directory separator to the given path string.
/// If the last character is already a separator nothing will be done.
Base_API void addsep(std::string& path);

/// Appends the given node to the path.
/// If the given path has no trailing separator one will be appended.
Base_API void addnode(std::string& path, const std::string& node);

/// Builds a path from a base and one or more nodes.
Base_API std::string makePath(const std::string& base, const std::string& node);

/// Normalizes a path, resolving `.` and `..` segments and
/// converting separators to the native style.
Base_API std::string normalize(const std::string& path);

/// Transcodes the path to windows native format if using windows
/// and if LibSourcey was compiled with Unicode support (SCY_UNICODE),
/// otherwise the path string is returned unchanged.
Base_API std::string transcode(const std::string& path);

/// Saves the given data buffer to the output file path.
/// Returns true on success, or if whiny is set then an
/// exception will be thrown on error.
Base_API bool savefile(const std::string& path, const char* data,
                       size_t size, bool whiny = false);


} // namespace fs
} // namespace scy


/// @\}
