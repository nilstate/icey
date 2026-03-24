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
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


namespace icy {
namespace fs {


/// The platform specific path separator string:
/// "/" on unix and "\\" on windows.
Base_API extern const char* separator;

/// The platform specific path separator character:
/// '/' on unix and '\\' on windows.
Base_API extern const char delimiter;

/// Returns the file name and extension part of the given path.
/// @param path Filesystem path to parse.
/// @return Filename component including extension (e.g. "file.txt").
Base_API std::string filename(std::string_view path);

/// Returns the file name without its extension.
/// @param path Filesystem path to parse.
/// @return Filename without the extension (e.g. "file").
Base_API std::string basename(std::string_view path);

/// Returns the directory part of the path.
/// @param path Filesystem path to parse.
/// @return Directory component including trailing separator (e.g. "/usr/local/").
Base_API std::string dirname(std::string_view path);

/// Returns the file extension part of the path.
/// @param path Filesystem path to parse.
/// @param includeDot If true (default), includes the leading dot (e.g. ".txt"); if false, the dot is stripped.
/// @return Extension string, or empty if the path has no extension.
Base_API std::string extname(std::string_view path, bool includeDot = true);

/// Returns true if the file or directory exists.
/// @param path Path to check.
/// @return True if the path exists on the filesystem.
Base_API bool exists(std::string_view path);

/// Returns true if the path refers to a directory.
/// @param path Path to check.
/// @return True if the path exists and is a directory.
Base_API bool isdir(std::string_view path);

/// Returns the size in bytes of the given file.
/// @param path Path to the file.
/// @return File size in bytes, or -1 if the file does not exist.
Base_API std::int64_t filesize(std::string_view path);

/// Populates `res` with the names of all entries in the given directory.
/// @param path Path to the directory to read.
/// @param res Vector to receive the list of entry names.
Base_API void readdir(std::string_view path, std::vector<std::string>& res);

/// Creates a single directory.
/// @param path Path of the directory to create.
/// @param mode Permission bits (default: 0755). Ignored on Windows.
Base_API void mkdir(std::string_view path, int mode = 0755);

/// Creates a directory and all missing parent directories.
/// @param path Path of the directory hierarchy to create.
/// @param mode Permission bits (default: 0755). Ignored on Windows.
Base_API void mkdirr(std::string_view path, int mode = 0755);

/// Removes an empty directory.
/// @param path Path of the directory to remove.
Base_API void rmdir(std::string_view path);

/// Deletes a file.
/// @param path Path of the file to delete.
Base_API void unlink(std::string_view path);

/// Renames or moves the given file to the target path.
/// @param path Source file path.
/// @param target Destination file path.
Base_API void rename(std::string_view path, std::string_view target);

/// Appends the platform-specific path separator to `path` if not already present.
/// @param path Path string to modify in place.
Base_API void addsep(std::string& path);

/// Appends a path node to `path`, inserting a separator if necessary.
/// @param path Base path string to modify in place.
/// @param node Directory or file name component to append.
Base_API void addnode(std::string& path, std::string_view node);

/// Joins a base path and a node component into a single path string.
/// @param base Base directory path.
/// @param node Directory or file name component to append.
/// @return Joined path string.
Base_API std::string makePath(std::string_view base, std::string_view node);

/// Normalizes a path by resolving `.` and `..` segments and converting
/// separators to the native platform style.
/// @param path Path string to normalize.
/// @return Normalized path string.
Base_API std::string normalize(std::string_view path);

/// Transcodes a path to the native platform format.
/// On Windows with `ICY_UNICODE` defined, converts to the Windows-native wide-to-narrow format.
/// On other platforms, returns the path unchanged.
/// @param path Path string to transcode.
/// @return Transcoded path string.
Base_API std::string transcode(std::string_view path);

/// Writes `size` bytes from `data` to the file at `path`, creating or overwriting it.
/// @param path Destination file path. Parent directories must already exist.
/// @param data Pointer to the data to write.
/// @param size Number of bytes to write.
/// @param whiny If true, throws a `std::runtime_error` on failure instead of returning false.
/// @return True on success, false on failure (when `whiny` is false).
Base_API bool savefile(std::string_view path, const char* data,
                       size_t size, bool whiny = false);


} // namespace fs
} // namespace icy


/// @}
