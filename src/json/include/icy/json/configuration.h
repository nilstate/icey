///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup json
/// @{


#pragma once


#include "icy/configuration.h"
#include "icy/json/json.h"
#include "icy/util.h"
#include <mutex>


namespace icy {
namespace json {


/// JSON configuration file
///
/// See base Configuration for all accessors
class JSON_API Configuration : public icy::Configuration
{
public:
    Configuration();
    virtual ~Configuration();

    /// Sets the file path and loads the configuration.
    /// @param path  Absolute or relative path to the JSON file.
    /// @param create Reserved for future use (currently unused).
    /// @throws std::runtime_error if the path is empty.
    virtual void load(const std::string& path, bool create = false);

    /// Reloads the configuration from the previously set path.
    /// Silently ignores parse errors (e.g. empty file).
    /// @param create Reserved for future use (currently unused).
    /// @throws std::runtime_error if the path has not been set.
    virtual void load(bool create = false);

    /// Writes the current JSON root to the file at the stored path.
    /// @throws std::runtime_error if the path is empty or the file cannot be written.
    virtual void save();

    /// Removes the top-level key @p key from the JSON root.
    /// @param key Key to remove.
    /// @return true if the key existed and was removed.
    virtual bool remove(const std::string& key);

    /// Removes all top-level keys whose names contain @p baseKey as a substring.
    /// @param baseKey Substring to match against key names.
    virtual void removeAll(const std::string& baseKey);

    /// Performs a global string substitution on the serialized JSON,
    /// replacing all occurrences of @p from with @p to, then re-parses.
    /// @param from Substring to find.
    /// @param to   Replacement string.
    virtual void replace(const std::string& from, const std::string& to);

    /// Populates @p keys with all top-level key names containing @p baseKey as a substring.
    /// @param keys    Output vector to append matching key names to.
    /// @param baseKey Filter substring; empty string matches all keys.
    virtual void keys(std::vector<std::string>& keys, const std::string& baseKey = "");

    /// Writes the pretty-printed JSON to @p ost with 4-space indentation.
    /// @param ost Output stream.
    virtual void print(std::ostream& ost);

    /// Returns the file path that was passed to load().
    virtual std::string path();

    /// Returns true if load() has been called at least once.
    virtual bool loaded();

    json::Value root;

protected:
    /// Retrieves the string value for @p key from the JSON root.
    /// @param key   Top-level JSON key.
    /// @param value Set to the string value if the key exists.
    /// @return true if the key was found, false otherwise.
    virtual bool getRaw(const std::string& key, std::string& value) const override;

    /// Stores @p value under @p key in the JSON root and emits PropertyChanged.
    /// @param key   Top-level JSON key.
    /// @param value String value to store.
    virtual void setRaw(const std::string& key, const std::string& value) override;

    bool _loaded;
    std::string _path;
    mutable std::mutex _mutex;
};


} // namespace json
} // namespace icy


/// @}
