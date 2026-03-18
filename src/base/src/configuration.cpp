///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/configuration.h"
#include "scy/error.h"
#include "scy/util.h"


namespace scy {


Configuration::Configuration()
{
}


Configuration::~Configuration()
{
}


bool Configuration::exists(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string tmp;
    return getRaw(key, tmp);
}


std::string Configuration::getString(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return value;
    else
        throw std::invalid_argument("Not found: " + key);
}


std::string Configuration::getString(const std::string& key,
                                     const std::string& defaultValue) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return value;
    else
        return defaultValue;
}


std::string Configuration::getRawString(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return value;
    else
        throw std::invalid_argument("Not found: " + key);
}


std::string Configuration::getRawString(const std::string& key,
                                        const std::string& defaultValue) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return value;
    else
        return defaultValue;
}


int Configuration::getInt(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return parseInt(value);
    else
        throw std::invalid_argument("Not found: " + key);
}


int Configuration::getInt(const std::string& key, int defaultValue) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return parseInt(value);
    else
        return defaultValue;
}


std::int64_t Configuration::getLargeInt(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return parseLargeInt(value);
    else
        throw std::invalid_argument("Not found: " + key);
}


std::int64_t Configuration::getLargeInt(const std::string& key,
                                        std::int64_t defaultValue) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return parseLargeInt(value);
    else
        return defaultValue;
}


double Configuration::getDouble(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return util::strtoi<double>(value);
    else
        throw std::invalid_argument("Not found: " + key);
}


double Configuration::getDouble(const std::string& key,
                                double defaultValue) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return util::strtoi<double>(value);
    else
        return defaultValue;
}


bool Configuration::getBool(const std::string& key) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return parseBool(value);
    else
        throw std::invalid_argument("Not found: " + key);
}


bool Configuration::getBool(const std::string& key, bool defaultValue) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string value;
    if (getRaw(key, value))
        return parseBool(value);
    else
        return defaultValue;
}


void Configuration::setString(const std::string& key, const std::string& value)
{
    std::lock_guard<std::mutex> guard(_mutex);

    setRaw(key, value);
}


void Configuration::setInt(const std::string& key, int value)
{
    std::lock_guard<std::mutex> guard(_mutex);

    setRaw(key, util::itostr<int>(value));
}


void Configuration::setLargeInt(const std::string& key, std::int64_t value)
{
    std::lock_guard<std::mutex> guard(_mutex);

    setRaw(key, util::itostr<std::int64_t>(value));
}


void Configuration::setDouble(const std::string& key, double value)
{
    std::lock_guard<std::mutex> guard(_mutex);

    setRaw(key, util::itostr<double>(value));
}


void Configuration::setBool(const std::string& key, bool value)
{
    std::lock_guard<std::mutex> guard(_mutex);

    setRaw(key, value ? "true" : "false");
}


int Configuration::parseInt(std::string_view value)
{
    if (value.compare(0, 2, "0x") == 0)
        return util::parseHex(value.substr(2));
    else
        return util::strtoi<int>(value);
}


std::int64_t Configuration::parseLargeInt(std::string_view value)
{
    if (value.compare(0, 2, "0x") == 0)
        return util::parseHex(value.substr(2));
    else
        return util::strtoi<std::int64_t>(value);
}


bool Configuration::parseBool(std::string_view value)
{
    if (value == "0")
        return false;
    else if (value == "1")
        return true;
    else if (util::icompare(value, "true") == 0)
        return true;
    else if (util::icompare(value, "yes") == 0)
        return true;
    else if (util::icompare(value, "on") == 0)
        return true;
    else if (util::icompare(value, "false") == 0)
        return false;
    else if (util::icompare(value, "no") == 0)
        return false;
    else if (util::icompare(value, "off") == 0)
        return false;
    else
        throw std::runtime_error("Syntax error: Cannot convert to boolean: " +
                                 std::string(value));
}


//
// Scoped Configuration
//


ScopedConfiguration::ScopedConfiguration(Configuration& config,
                                         const std::string& currentScope,
                                         const std::string& defaultScope)
    : config(config)
    , currentScope(currentScope)
    , defaultScope(defaultScope)
{
}


ScopedConfiguration::ScopedConfiguration(const ScopedConfiguration& r)
    : config(r.config)
    , currentScope(r.currentScope)
    , defaultScope(r.defaultScope)
{
}


std::string ScopedConfiguration::getString(const std::string& key,
                                           const std::string& defaultValue,
                                           bool forceDefaultScope) const
{
    return forceDefaultScope
               ? config.getString(getDafaultKey(key), defaultValue)
               : config.getString(
                     getCurrentScope(key),
                     config.getString(getDafaultKey(key), defaultValue));
}


int ScopedConfiguration::getInt(const std::string& key, int defaultValue,
                                bool forceDefaultScope) const
{
    return forceDefaultScope
               ? config.getInt(getDafaultKey(key), defaultValue)
               : config.getInt(getCurrentScope(key),
                               config.getInt(getDafaultKey(key), defaultValue));
}


double ScopedConfiguration::getDouble(const std::string& key,
                                      double defaultValue,
                                      bool forceDefaultScope) const
{
    return forceDefaultScope
               ? config.getDouble(getDafaultKey(key), defaultValue)
               : config.getDouble(
                     getCurrentScope(key),
                     config.getDouble(getDafaultKey(key), defaultValue));
}


bool ScopedConfiguration::getBool(const std::string& key, bool defaultValue,
                                  bool forceDefaultScope) const
{
    return forceDefaultScope ? config.getBool(getDafaultKey(key), defaultValue)
                             : config.getBool(getCurrentScope(key),
                                              config.getBool(getDafaultKey(key),
                                                             defaultValue));
}


void ScopedConfiguration::setString(const std::string& key,
                                    const std::string& value, bool forceDefaultScope)
{
    config.setString(getScopedKey(key, forceDefaultScope), value);
}


void ScopedConfiguration::setInt(const std::string& key, int value,
                                 bool forceDefaultScope)
{
    config.setInt(getScopedKey(key, forceDefaultScope), value);
}


void ScopedConfiguration::setDouble(const std::string& key, double value,
                                    bool forceDefaultScope)
{
    config.setDouble(getScopedKey(key, forceDefaultScope), value);
}


void ScopedConfiguration::setBool(const std::string& key, bool value,
                                  bool forceDefaultScope)
{
    config.setBool(getScopedKey(key, forceDefaultScope), value);
}


std::string ScopedConfiguration::getCurrentScope(const std::string& key) const
{
    return currentScope + key;
}


std::string ScopedConfiguration::getDafaultKey(const std::string& key) const
{
    return defaultScope + key;
}


std::string ScopedConfiguration::getScopedKey(const std::string& key,
                                              bool forceDefaultScope) const
{
    return forceDefaultScope ? getDafaultKey(key) : getCurrentScope(key);
}


} // namespace scy


/// @\}
