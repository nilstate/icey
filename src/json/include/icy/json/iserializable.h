///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup json
/// @{


#pragma once


#include "icy/json/json.h"
#include <ostream>


namespace icy {
namespace json {


class ISerializable
{
public:
    virtual ~ISerializable() = default;
    virtual void serialize(json::Value& root) = 0;
    virtual void deserialize(json::Value& root) = 0;
};


inline bool serialize(ISerializable* pObj, std::string& output)
{
    if (pObj == nullptr)
        return false;

    json::Value serializeRoot;
    pObj->serialize(serializeRoot);
    output = serializeRoot.dump(4);
    return true;
}


inline bool deserialize(ISerializable* pObj, std::string& input)
{
    if (pObj == nullptr)
        return false;

    try {
        json::Value deserializeRoot = json::Value::parse(input.begin(), input.end());
        pObj->deserialize(deserializeRoot);
    } catch (const std::exception&) {
        return false;
    }

    return true;
}


} // namespace json
} // namespace icy
