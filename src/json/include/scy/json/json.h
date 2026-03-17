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
#include "scy/error.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <fstream>


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


// Import the `json` type and friends to our namespace
using value = nlohmann::json;


inline void loadFile(const std::string& path, json::value& root)
{
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ifstream::in);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open input file: " + path);

    root = json::value::parse(ifs); // thorws std::invalid_argument
}


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


//inline void stringify(const json::value& root, std::string& output, bool pretty = false)
//{
//    if (pretty) {
//        json::StyledWriter writer;
//        output = writer.write(root);
//    } else {
//        json::FastWriter writer;
//        output = writer.write(root); // NOTE: The FastWriter appends a newline
//                                     // character which we don't want.
//        if (output.size() > 0)
//            output.resize(output.size() - 1);
//    }
//}
//
//
//inline std::string stringify(const json::value& root, bool pretty = false)
//{
//    std::string output;
//    stringify(root, output, pretty);
//    return output;
//}


inline void assertMember(const json::value& root, const std::string& name)
{
    if (root.find(name) == root.end())
        throw std::runtime_error("A '" + name + "' member is required.");
}


inline void countNestedKeys(json::value& root, const std::string& key,
                            int& count, int depth = 0)
{
    if (!root.is_object() && !root.is_array())
        return;
    depth++;
    for (auto it = root.begin(); it != root.end(); ++it) {
        if ((*it).is_object() && (*it).find(key) != (*it).end())
            count++;
        countNestedKeys(*it, key, count, depth);
    }
}


inline bool hasNestedKey(json::value& root, const std::string& key, int depth = 0)
{
    if (!root.is_object() && !root.is_array())
        return false;
    depth++;
    for (auto it = root.begin(); it != root.end(); it++) {
        if ((*it).is_object() && (*it).find(key) != (*it).end())
            return true;
        if (hasNestedKey(*it, key, depth))
            return true;
    }
    return false;
}


/// Only works for objects with string values.
/// Key or value may be empty for selecting wildcard values.
/// If partial is false substring matches will be accepted.
/// Result must be a reference to a pointer or the root value's
/// internal reference will also be changed when the result is
/// assigned. Further investigation is required.
inline bool findNestedObjectWithProperty(
    json::value& root, json::value*& result, const std::string& key,
    const std::string& value, bool partial = true, int index = 0,
    int depth = 0)
{
    depth++;
    if (root.is_object()) {
        for (auto it = root.begin(); it != root.end(); it++) {
            json::value& test = it.value();
            if (test.is_null())
                continue;
            else if (test.is_string() &&
                     (key.empty() || it.value() == key) &&
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
                                                    index, depth))
                return true;
        }
    } else if (root.is_array()) {
        for (size_t i = 0; i < root.size(); i++) {
            json::value& test = root[(int)i];
            if (!test.is_null() && (test.is_object() || test.is_array()) &&
                findNestedObjectWithProperty(root[(int)i], result, key, value,
                                             partial, index, depth))
                return true;
        }
    }
    return false;
}


} // namespace json
} // namespace scy


/// @\}
