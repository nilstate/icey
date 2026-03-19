///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/filesystem.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#if defined(_MSC_VER) && defined(ICY_UNICODE)
#include <codecvt>
#include <locale>
#include <memory>
#include <windows.h>
#endif


namespace stdfs = std::filesystem;


namespace icy {
namespace fs {


#ifdef ICY_WIN
const char delimiter = '\\';
const char* separator = "\\";
#else
const char delimiter = '/';
const char* separator = "/";
#endif


std::string filename(std::string_view path)
{
    return stdfs::path(path).filename().string();
}


std::string dirname(std::string_view path)
{
    auto parent = stdfs::path(path).parent_path();
    if (parent.empty())
        return ".";
    return parent.string();
}


std::string basename(std::string_view path)
{
    return stdfs::path(path).stem().string();
}


std::string extname(std::string_view path, bool includeDot)
{
    auto ext = stdfs::path(path).extension().string();
    if (ext.empty())
        return "";
    if (!includeDot && !ext.empty() && ext[0] == '.')
        return ext.substr(1);
    return ext;
}


bool exists(std::string_view path)
{
    std::error_code ec;
    return stdfs::exists(stdfs::path(path), ec);
}


bool isdir(std::string_view path)
{
    std::error_code ec;
    return stdfs::is_directory(stdfs::path(path), ec);
}


std::int64_t filesize(std::string_view path)
{
    std::error_code ec;
    auto size = stdfs::file_size(stdfs::path(path), ec);
    if (ec)
        return -1;
    return static_cast<std::int64_t>(size);
}


void readdir(std::string_view path, std::vector<std::string>& res)
{
    std::error_code ec;
    for (const auto& entry : stdfs::directory_iterator(stdfs::path(path), ec)) {
        res.push_back(entry.path().filename().string());
    }
    if (ec)
        throw std::runtime_error("Filesystem error: scandir failed: " + std::string(path));
}


void mkdir(std::string_view path, int /*mode*/)
{
    std::error_code ec;
    stdfs::create_directory(stdfs::path(path), ec);
    if (ec)
        throw std::runtime_error("Filesystem error: mkdir failed: " + std::string(path) + ": " + ec.message());
}


void mkdirr(std::string_view path, int /*mode*/)
{
    std::error_code ec;
    stdfs::create_directories(stdfs::path(path), ec);
    if (ec)
        throw std::runtime_error("Filesystem error: mkdir failed: " + std::string(path) + ": " + ec.message());
}


void rmdir(std::string_view path)
{
    std::error_code ec;
    stdfs::remove(stdfs::path(path), ec);
    if (ec)
        throw std::runtime_error("Filesystem error: rmdir failed: " + std::string(path) + ": " + ec.message());
}


void unlink(std::string_view path)
{
    std::error_code ec;
    stdfs::remove(stdfs::path(path), ec);
    if (ec)
        throw std::runtime_error("Filesystem error: unlink failed: " + std::string(path) + ": " + ec.message());
}


void rename(std::string_view path, std::string_view target)
{
    std::error_code ec;
    stdfs::rename(stdfs::path(path), stdfs::path(target), ec);
    if (ec)
        throw std::runtime_error("Filesystem error: rename failed: " + std::string(path) + ": " + ec.message());
}


void addsep(std::string& path)
{
    if (!path.empty() && path.back() != '/' && path.back() != '\\')
        path.append(fs::separator, 1);
}


void addnode(std::string& path, std::string_view node)
{
    fs::addsep(path);
    path += node;
}


std::string makePath(std::string_view base, std::string_view node)
{
    return (stdfs::path(base) / stdfs::path(node)).string();
}


std::string normalize(std::string_view path)
{
    return stdfs::path(path).lexically_normal().string();
}


std::string transcode(std::string_view path)
{
#if defined(_MSC_VER) && defined(ICY_UNICODE)
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    std::wstring uniPath = convert.from_bytes(std::string(path));
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
    return std::string(path);
}


bool savefile(std::string_view path, const char* data, size_t size,
              bool whiny)
{
    std::ofstream ofs(std::string(path), std::ios_base::binary | std::ios_base::out);
    if (ofs.is_open())
        ofs.write(data, size);
    else {
        if (whiny)
            throw std::runtime_error("Cannot save file: " + std::string(path));
        return false;
    }
    return true;
}


} // namespace fs
} // namespace icy


/// @\}
