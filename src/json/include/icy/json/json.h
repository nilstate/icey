///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup json JSON module
///
/// JSON serialization using nlohmann/json.
/// @{


#pragma once


#include "icy/base.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <vector>


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(JSON_EXPORTS)
#define JSON_API __declspec(dllexport)
#else
#define JSON_API __declspec(dllimport)
#endif
#else
#define JSON_API // nothing
#endif


namespace icy {
/// JSON value aliases, serialization helpers, and file utilities built on nlohmann/json.
namespace json {


using Value = nlohmann::json;


/// Load a JSON file into a value. Throws on missing file or parse error.
inline void loadFile(const std::string& path, json::Value& root)
{
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ifstream::in);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open input file: " + path);

    root = json::Value::parse(ifs);
}


/// Save a JSON value to a file. Throws on write error.
inline void saveFile(const std::string& path, const json::Value& root, int indent = 4)
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
inline void assertMember(const json::Value& root, const std::string& name)
{
    if (!root.contains(name))
        throw std::runtime_error("A '" + name + "' member is required.");
}


/// Count how many nested objects contain the given key.
inline void countNestedKeys(const json::Value& root, const std::string& key, int& count)
{
    if (!root.is_object() && !root.is_array())
        return;
    for (const auto& elem : root) {
        if (elem.is_object() && elem.contains(key))
            count++;
        countNestedKeys(elem, key, count);
    }
}


/// Return true if any nested object contains the given key.
inline bool hasNestedKey(const json::Value& root, const std::string& key)
{
    if (!root.is_object() && !root.is_array())
        return false;
    for (const auto& elem : root) {
        if (elem.is_object() && elem.contains(key))
            return true;
        if (hasNestedKey(elem, key))
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
    json::Value& root, json::Value*& result, std::string_view key,
    std::string_view value, bool partial = true, int index = 0)
{
    if (root.is_object()) {
        for (auto& [k, test] : root.items()) {
            if (test.is_null())
                continue;
            else if (test.is_string() &&
                     (key.empty() || k == key) &&
                     (value.empty() ||
                      (!partial ? test.get<std::string>() == value
                                : test.get<std::string>().find(value) != std::string::npos))) {
                if (index == 0) {
                    result = &root;
                    return true;
                } else
                    index--;
            } else if ((test.is_object() || test.is_array()) &&
                       findNestedObjectWithProperty(test,
                                                    result, key, value, partial,
                                                    index))
                return true;
        }
    } else if (root.is_array()) {
        for (size_t i = 0; i < root.size(); i++) {
            json::Value& test = root[i];
            if (!test.is_null() && (test.is_object() || test.is_array()) &&
                findNestedObjectWithProperty(root[i], result, key, value,
                                             partial, index))
                return true;
        }
    }
    return false;
}


} // namespace json
} // namespace icy


/// @}
