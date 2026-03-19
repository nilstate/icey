///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
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
namespace archo {


struct Archo_API ZipFile
{
    ZipFile();
    explicit ZipFile(const std::filesystem::path& file);
    ~ZipFile();

    ZipFile(const ZipFile&) = delete;
    ZipFile& operator=(const ZipFile&) = delete;
    ZipFile(ZipFile&&) = delete;
    ZipFile& operator=(ZipFile&&) = delete;

    void open(const std::filesystem::path& file);
    [[nodiscard]] bool opened() const;
    void close();

    /// Extracts the archive contents to the given directory path.
    void extract(const std::filesystem::path& path);
    [[nodiscard]] bool extractCurrentFile(const std::filesystem::path& path, bool whiny = true);

    [[nodiscard]] bool goToFirstFile();
    [[nodiscard]] bool goToNextFile();

    void openCurrentFile();
    void closeCurrentFile();

    [[nodiscard]] std::string currentFileName();

    struct FileInfo
    {
        std::string path;
        size_t compressedSize;
        size_t uncompressedSize;
    };

    std::vector<FileInfo> info;
    unzFile fp;
};


} // namespace archo
} // namespace icy


/// @\}
