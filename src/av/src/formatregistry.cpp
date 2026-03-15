///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/formatregistry.h"


using std::endl;


namespace scy {
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


Format& FormatRegistry::get(const std::string& name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    return findByName(name);
}


Format& FormatRegistry::getByID(const std::string& id)
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (auto& fmt : _formats) {
        if (fmt.id == id)
            return fmt;
    }
    throw std::runtime_error("Not found: No media format type: " + id);
}


Format& FormatRegistry::getOrDefault(const std::string& name)
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


bool FormatRegistry::exists(const std::string& name)
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


bool FormatRegistry::unregisterFormat(const std::string& name)
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


void FormatRegistry::setDefault(const std::string& name)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _default = name;
}


// Private: must be called with _mutex held
Format& FormatRegistry::findByName(const std::string& name)
{
    for (auto& fmt : _formats) {
        if (fmt.name == name)
            return fmt;
    }
    throw std::runtime_error("Not found: No media format for: " + name);
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
} // namespace scy


/// @\}
