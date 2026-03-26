///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup archo
/// @{


#pragma once


#include "icy/archo/archo.h"
#include <filesystem>
#include <string>
#include <unzip.h> // zlib
#include <vector>


namespace icy {
/// ZIP archive handling utilities and related helpers.
namespace archo {


/// ZIP archive reader and writer
struct Archo_API ZipFile
{
    /// Constructs an unopened ZipFile. Call open() before use.
    ZipFile();

    /// Constructs a ZipFile and immediately opens the archive at @p file.
    /// @param file Path to the ZIP archive to open.
    explicit ZipFile(const std::filesystem::path& file);

    /// Closes the archive if still open.
    ~ZipFile();

    ZipFile(const ZipFile&) = delete;
    ZipFile& operator=(const ZipFile&) = delete;
    ZipFile(ZipFile&&) = delete;
    ZipFile& operator=(ZipFile&&) = delete;

    /// Opens the archive at @p file, closing any previously opened archive.
    /// Populates the `info` vector with metadata for every entry.
    /// @param file Path to the ZIP archive.
    /// @throws std::runtime_error if the file cannot be opened.
    void open(const std::filesystem::path& file);

    /// Returns true if the archive is currently open.
    [[nodiscard]] bool opened() const;

    /// Closes the archive and releases the underlying file handle.
    void close();

    /// Extracts the archive contents to the given directory path.
    /// @param path Destination directory; created automatically if necessary.
    /// @throws std::runtime_error if the archive is not open or a read error occurs.
    void extract(const std::filesystem::path& path);

    /// Extracts the current file entry to @p path.
    /// Validates each entry against path-traversal attacks before writing.
    /// @param path Destination base directory.
    /// @param whiny If true, re-throws on error; otherwise returns false.
    /// @return true on success, false if @p whiny is false and an error occurred.
    [[nodiscard]] bool extractCurrentFile(const std::filesystem::path& path, bool whiny = true);

    /// Moves the internal cursor to the first file entry in the archive.
    /// @return true on success, false if the archive is empty or an error occurred.
    [[nodiscard]] bool goToFirstFile();

    /// Advances the internal cursor to the next file entry.
    /// @return true if another entry exists, false at end-of-list.
    [[nodiscard]] bool goToNextFile();

    /// Opens the current file entry for reading.
    /// @throws std::runtime_error on failure.
    void openCurrentFile();

    /// Closes the current file entry.
    /// @throws std::runtime_error on failure.
    void closeCurrentFile();

    /// Returns the name (relative path) of the current file entry.
    /// @return Entry name as reported by the ZIP directory.
    [[nodiscard]] std::string currentFileName();

    /// Metadata for a file entry within a ZIP archive
    struct FileInfo
    {
        std::string path;           ///< Relative path of the entry within the archive.
        size_t compressedSize;      ///< Compressed size in bytes.
        size_t uncompressedSize;    ///< Uncompressed size in bytes.
    };

    std::vector<FileInfo> info;
    unzFile fp;
};


} // namespace archo
} // namespace icy


/// @}
