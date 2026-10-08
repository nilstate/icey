///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup archo
/// @{


#include "icy/archo/zipfile.h"
#include "icy/logger.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>




namespace icy {
namespace archo {


namespace internal {

constexpr uint64_t kPerFileMax = 2ULL * 1024 * 1024 * 1024;
constexpr uint64_t kArchiveMax = 4ULL * 1024 * 1024 * 1024;
constexpr uint64_t kEntryMax = 100000;
constexpr unsigned int kZipDirectoryAttribute = 0x10;

std::string errmsg(int code)
{
    switch (code) {
        case UNZ_END_OF_LIST_OF_FILE:
            return "end of file list reached";
        case UNZ_EOF:
            return "end of file reached";
        case UNZ_PARAMERROR:
            return "invalid parameter given";
        case UNZ_BADZIPFILE:
            return "bad zip file";
        case UNZ_INTERNALERROR:
            return "internal error";
        case UNZ_CRCERROR:
            return "crc error, file is corrupt";
        case UNZ_ERRNO:
            return strerror(code);
        default:
            return "unknown error";
    }
}

void throwError(const std::string& error, int code = 0)
{
    std::string msg;
    if (!error.empty()) {
        msg += error;
    }
    if (code) {
        msg += ": ";
        msg += errmsg(code);
    }
    throw std::runtime_error(msg);
}

void api(const char* what, int ret)
{
    if (ret != UNZ_OK)
        throwError(what, ret);
}

} // namespace internal


ZipFile::ZipFile()
    : fp(nullptr)
{
}


ZipFile::ZipFile(const std::filesystem::path& file)
    : fp(nullptr)
{
    this->open(file);
}


ZipFile::~ZipFile()
{
    this->close();
}


bool ZipFile::opened() const
{
    return this->fp != nullptr;
}


void ZipFile::open(const std::filesystem::path& file)
{
    this->close();
    this->fp = unzOpen(file.string().c_str());
    if (this->fp == nullptr)
        internal::throwError("Cannot open archive file: " + file.string());

    for (int ret = unzGoToFirstFile(this->fp); ret == UNZ_OK;
         ret = unzGoToNextFile(this->fp)) {
        unz_file_info fileInfo;
        char fileName[1025] = {0};
        internal::api("unzGetCurrentFileInfo",
                      unzGetCurrentFileInfo(this->fp, &fileInfo, fileName, 1024,
                                            nullptr, 0, nullptr, 0));
        fileName[1024] = '\0';

        FileInfo finfo;
        finfo.path = fileName;
        finfo.compressedSize = static_cast<size_t>(fileInfo.compressed_size);
        finfo.uncompressedSize = static_cast<size_t>(fileInfo.uncompressed_size);
        this->info.push_back(finfo);

        LTrace("Zip file contains: ", fileName);
    }

    unzGoToFirstFile(this->fp); // rewind
}


void ZipFile::close()
{
    if (this->opened()) {
        internal::api("unzClose", unzClose(this->fp));
        this->fp = nullptr;
    }
}


void ZipFile::extract(const std::filesystem::path& path)
{
    LTrace("Extracting zip to: ", path.string());

    if (!opened())
        throw std::runtime_error("The archive must be opened for extraction.");

    if (!goToFirstFile())
        throw std::runtime_error("Cannot read the source archive.");

    _totalWritten = 0;
    _entriesExtracted = 0;

    while (true) {
        (void)extractCurrentFile(path, true);
        if (!goToNextFile())
            break;
    }
}


bool ZipFile::extractCurrentFile(const std::filesystem::path& path, bool whiny)
{
    int ret;
    unz_file_info finfo;
    char fname[1025] = {0};

    try {
        internal::api("unzGetCurrentFileInfo",
                      unzGetCurrentFileInfo(this->fp, &finfo, fname, 1024,
                                            nullptr, 0, nullptr, 0));
        fname[1024] = '\0';
        if (++_entriesExtracted > internal::kEntryMax)
            throw std::runtime_error("zip bomb: entry limit exceeded");

        auto outPath = path / fname;

        // Validate against path traversal attacks (e.g. "../../../etc/passwd")
        auto canonical = std::filesystem::weakly_canonical(outPath);
        auto canonicalBase = std::filesystem::weakly_canonical(path);
        auto [rootEnd, canonEnd] = std::mismatch(
            canonicalBase.begin(), canonicalBase.end(),
            canonical.begin(), canonical.end());
        (void)canonEnd;
        if (rootEnd != canonicalBase.end())
            throw std::runtime_error("Zip entry attempts path traversal: " + std::string(fname));

        LTrace("Extracting asset: ", outPath.string());

        const size_t nameLen = strlen(fname);
        if (finfo.external_fa & internal::kZipDirectoryAttribute ||
            (nameLen > 0 && fname[nameLen - 1] == '/')) {
            LTrace("Create directory: ", outPath.string());
            std::filesystem::create_directories(outPath);
        }

        // Create file
        else {
            LTrace("Create file: ", outPath.string());

            // Note: If this fails the file we are trying
            // to write may be in use on the filesystem.
            openCurrentFile();

            std::ofstream ofs(outPath, std::ios::binary | std::ios::out);

            // FIX: FILE_ATTRIBUTE_DIRECTORY can be inconsistent, so we
            // need to be ready to create directories if the output file
            // fails to open.
            if (!ofs.is_open()) {
                std::filesystem::create_directories(outPath.parent_path());
                ofs.open(outPath);
            }

            if (!ofs.is_open())
                throw std::runtime_error("Cannot open zip output file: " + outPath.string());

            char buffer[16384];
            uint64_t written = 0;
            while ((ret = unzReadCurrentFile(this->fp, buffer, sizeof(buffer))) > 0) {
                written += static_cast<uint64_t>(ret);
                _totalWritten += static_cast<uint64_t>(ret);
                if (written > internal::kPerFileMax ||
                    _totalWritten > internal::kArchiveMax) {
                    throw std::runtime_error("zip bomb: size limit exceeded");
                }
                ofs.write(buffer, ret);
            }

            ofs.close();

            internal::api("unzReadCurrentFile", ret); // throw file read errors
            closeCurrentFile();
        }
    } catch (std::exception& exc) {
        LError("Cannot unzip file: ", exc.what());
        if (whiny)
            throw;
        return false;
    }
    return true;
}


bool ZipFile::goToFirstFile()
{
    return unzGoToFirstFile(this->fp) == UNZ_OK;
}


bool ZipFile::goToNextFile()
{
    return unzGoToNextFile(this->fp) == UNZ_OK;
}


void ZipFile::openCurrentFile()
{
    internal::api("unzOpenCurrentFile", unzOpenCurrentFile(this->fp));
}


void ZipFile::closeCurrentFile()
{
    internal::api("unzCloseCurrentFile", unzCloseCurrentFile(this->fp));
}


std::string ZipFile::currentFileName()
{
    char buf[1025] = {0};
    internal::api("unzGetCurrentFileInfo",
                  unzGetCurrentFileInfo(this->fp, nullptr, buf, 1024,
                                        nullptr, 0, nullptr, 0));
    buf[1024] = '\0';
    return buf;
}


} // namespace archo
} // namespace icy


/// @}
