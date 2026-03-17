///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/filesystem.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#if defined(_MSC_VER) && defined(SCY_UNICODE)
#include <codecvt>
#include <locale>
#include <memory>
#endif


namespace stdfs = std::filesystem;


namespace scy {
namespace fs {


#ifdef SCY_WIN
const char delimiter = '\\';
const char* separator = "\\";
#else
const char delimiter = '/';
const char* separator = "/";
#endif


std::string filename(const std::string& path)
{
    return stdfs::path(path).filename().string();
}


std::string dirname(const std::string& path)
{
    auto parent = stdfs::path(path).parent_path();
    if (parent.empty())
        return ".";
    return parent.string();
}


std::string basename(const std::string& path)
{
    return stdfs::path(path).stem().string();
}


std::string extname(const std::string& path, bool includeDot)
{
    auto ext = stdfs::path(path).extension().string();
    if (ext.empty())
        return "";
    if (!includeDot && !ext.empty() && ext[0] == '.')
        return ext.substr(1);
    return ext;
}


bool exists(const std::string& path)
{
    std::error_code ec;
    return stdfs::exists(path, ec);
}


bool isdir(const std::string& path)
{
    std::error_code ec;
    return stdfs::is_directory(path, ec);
}


std::int64_t filesize(const std::string& path)
{
    std::error_code ec;
    auto size = stdfs::file_size(path, ec);
    if (ec)
        return -1;
    return static_cast<std::int64_t>(size);
}


void readdir(const std::string& path, std::vector<std::string>& res)
{
    std::error_code ec;
    for (const auto& entry : stdfs::directory_iterator(path, ec)) {
        res.push_back(entry.path().filename().string());
    }
    if (ec)
        throw std::runtime_error("Filesystem error: scandir failed: " + path);
}


void mkdir(const std::string& path, int /*mode*/)
{
    std::error_code ec;
    stdfs::create_directory(path, ec);
    if (ec)
        throw std::runtime_error("Filesystem error: mkdir failed: " + path + ": " + ec.message());
}


void mkdirr(const std::string& path, int /*mode*/)
{
    std::error_code ec;
    stdfs::create_directories(path, ec);
    if (ec)
        throw std::runtime_error("Filesystem error: mkdir failed: " + path + ": " + ec.message());
}


void rmdir(const std::string& path)
{
    std::error_code ec;
    stdfs::remove(path, ec);
    if (ec)
        throw std::runtime_error("Filesystem error: rmdir failed: " + path + ": " + ec.message());
}


void unlink(const std::string& path)
{
    std::error_code ec;
    stdfs::remove(path, ec);
    if (ec)
        throw std::runtime_error("Filesystem error: unlink failed: " + path + ": " + ec.message());
}


void rename(const std::string& path, const std::string& target)
{
    std::error_code ec;
    stdfs::rename(path, target, ec);
    if (ec)
        throw std::runtime_error("Filesystem error: rename failed: " + path + ": " + ec.message());
}


void addsep(std::string& path)
{
    if (!path.empty() && path.back() != '/' && path.back() != '\\')
        path.append(fs::separator, 1);
}


void addnode(std::string& path, const std::string& node)
{
    fs::addsep(path);
    path += node;
}


std::string makePath(const std::string& base, const std::string& node)
{
    return (stdfs::path(base) / node).string();
}


std::string normalize(const std::string& path)
{
    return stdfs::path(path).lexically_normal().string();
}


std::string transcode(const std::string& path)
{
#if defined(_MSC_VER) && defined(SCY_UNICODE)
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    std::wstring uniPath = convert.from_bytes(path);
    DWORD len = WideCharToMultiByte(
        CP_ACP, WC_NO_BEST_FIT_CHARS, uniPath.c_str(),
        static_cast<int>(uniPath.length()), nullptr, 0, nullptr, nullptr);
    if (len > 0) {
        auto buffer = std::make_unique<char[]>(len);
        DWORD rc = WideCharToMultiByte(
            CP_ACP, WC_NO_BEST_FIT_CHARS, uniPath.c_str(),
            static_cast<int>(uniPath.length()), buffer.get(),
            static_cast<int>(len), nullptr, nullptr);
        if (rc) {
            return std::string(buffer.get(), len);
        }
    }
#endif
    return path;
}


bool savefile(const std::string& path, const char* data, size_t size,
              bool whiny)
{
    std::ofstream ofs(path, std::ios_base::binary | std::ios_base::out);
    if (ofs.is_open())
        ofs.write(data, size);
    else {
        if (whiny)
            throw std::runtime_error("Cannot save file: " + path);
        return false;
    }
    return true;
}


} // namespace fs
} // namespace scy


/// @\}
