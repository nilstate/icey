///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup json
/// @{


#pragma once


#include "scy/base.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <vector>


// Shared library exports
#if defined(SCY_WIN) && defined(SCY_SHARED_LIBRARY)
#if defined(JSON_EXPORTS)
#define JSON_API __declspec(dllexport)
#else
#define JSON_API __declspec(dllimport)
#endif
#else
#define JSON_API // nothing
#endif


namespace scy {
namespace json {


using value = nlohmann::json;


/// Load a JSON file into a value. Throws on missing file or parse error.
inline void loadFile(const std::string& path, json::value& root)
{
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ifstream::in);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open input file: " + path);

    root = json::value::parse(ifs);
}


/// Save a JSON value to a file. Throws on write error.
inline void saveFile(const std::string& path, const json::value& root, int indent = 4)
{
    std::ofstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs.is_open())
        throw std::runtime_error("Cannot open output file: " + path);

    if (indent > 0)
        ofs << root.dump(indent);
    else
        ofs << root.dump();
    ofs.close();
}


/// Assert that a required member exists. Throws if missing.
inline void assertMember(const json::value& root, const std::string& name)
{
    if (!root.contains(name))
        throw std::runtime_error("A '" + name + "' member is required.");
}


/// Count how many nested objects contain the given key.
inline void countNestedKeys(const json::value& root, const std::string& key, int& count)
{
    if (!root.is_object() && !root.is_array())
        return;
    for (auto it = root.begin(); it != root.end(); ++it) {
        if ((*it).is_object() && (*it).contains(key))
            count++;
        countNestedKeys(*it, key, count);
    }
}


/// Return true if any nested object contains the given key.
inline bool hasNestedKey(const json::value& root, const std::string& key)
{
    if (!root.is_object() && !root.is_array())
        return false;
    for (auto it = root.begin(); it != root.end(); ++it) {
        if ((*it).is_object() && (*it).contains(key))
            return true;
        if (hasNestedKey(*it, key))
            return true;
    }
    return false;
}


/// Find a nested object whose property matches the given key/value.
///
/// Key or value may be empty for wildcard matching.
/// If partial is true, substring matches are accepted for string values.
/// The index parameter selects the Nth match (0 = first).
///
/// Returns true if found, with result pointing to the matching object.
inline bool findNestedObjectWithProperty(
    json::value& root, json::value*& result, const std::string& key,
    const std::string& value, bool partial = true, int index = 0)
{
    if (root.is_object()) {
        for (auto it = root.begin(); it != root.end(); ++it) {
            json::value& test = it.value();
            if (test.is_null())
                continue;
            else if (test.is_string() &&
                     (key.empty() || it.key() == key) &&
                     (value.empty() ||
                      (!partial ? test.get<std::string>() == value
                                : test.get<std::string>().find(value) != std::string::npos))) {
                if (index == 0) {
                    result = &root;
                    return true;
                } else
                    index--;
            } else if ((test.is_object() || test.is_array()) &&
                       findNestedObjectWithProperty(it.value(),
                                                    result, key, value, partial,
                                                    index))
                return true;
        }
    } else if (root.is_array()) {
        for (size_t i = 0; i < root.size(); i++) {
            json::value& test = root[i];
            if (!test.is_null() && (test.is_object() || test.is_array()) &&
                findNestedObjectWithProperty(root[i], result, key, value,
                                             partial, index))
                return true;
        }
    }
    return false;
}


} // namespace json
} // namespace scy


/// @\}
