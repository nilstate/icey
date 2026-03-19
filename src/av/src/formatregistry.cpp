///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/formatregistry.h"

#include <string_view>



namespace icy {
namespace av {


FormatRegistry::FormatRegistry()
{
}


FormatRegistry::~FormatRegistry() noexcept
{
}


FormatRegistry& FormatRegistry::instance()
{
    static FormatRegistry instance;
    return instance;
}


Format& FormatRegistry::get(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    return findByName(name);
}


Format& FormatRegistry::getByID(std::string_view id)
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (auto& fmt : _formats) {
        if (fmt.id == id)
            return fmt;
    }
    throw std::runtime_error("Not found: No media format type: " + std::string(id));
}


Format& FormatRegistry::getOrDefault(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (auto& fmt : _formats) {
        if (fmt.name == name)
            return fmt;
    }
    return defaultLocked();
}


Format& FormatRegistry::getDefault()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return defaultLocked();
}


bool FormatRegistry::exists(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (const auto& fmt : _formats) {
        if (fmt.name == name)
            return true;
    }
    return false;
}


void FormatRegistry::clear()
{
    std::lock_guard<std::mutex> guard(_mutex);
    _formats.clear();
}


FormatList FormatRegistry::formats() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _formats;
}


void FormatRegistry::registerFormat(const Format& format)
{
    std::lock_guard<std::mutex> guard(_mutex);
    // Remove existing format with the same name (unlocked)
    for (auto it = _formats.begin(); it != _formats.end(); ++it) {
        if (it->name == format.name) {
            if (_default == format.name)
                _default = "";
            _formats.erase(it);
            break;
        }
    }
    _formats.push_back(format);
}


bool FormatRegistry::unregisterFormat(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (auto it = _formats.begin(); it != _formats.end(); ++it) {
        if (it->name == name) {
            _formats.erase(it);
            if (_default == name)
                _default = "";
            return true;
        }
    }
    return false;
}


void FormatRegistry::setDefault(std::string_view name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _default = name;
}


// Private: must be called with _mutex held
Format& FormatRegistry::findByName(std::string_view name)
{
    for (auto& fmt : _formats) {
        if (fmt.name == name)
            return fmt;
    }
    throw std::runtime_error("Not found: No media format for: " + std::string(name));
}


// Private: must be called with _mutex held
Format& FormatRegistry::defaultLocked()
{
    if (!_default.empty())
        return findByName(_default);
    if (!_formats.empty())
        return *_formats.begin();
    throw std::runtime_error("Not found: No default media format.");
}


} // namespace av
} // namespace icy


/// @\}
