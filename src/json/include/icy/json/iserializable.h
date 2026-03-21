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


/// Abstract interface for JSON-serializable objects
class ISerializable
{
public:
    virtual ~ISerializable() = default;

    /// Serializes this object's state into @p root.
    /// @param root JSON object to populate.
    virtual void serialize(json::Value& root) = 0;

    /// Populates this object's state from @p root.
    /// @param root JSON object previously produced by serialize().
    virtual void deserialize(json::Value& root) = 0;
};


/// Serializes @p pObj to a pretty-printed JSON string.
/// @param pObj  Object to serialize; must not be null.
/// @param output Receives the 4-space indented JSON string.
/// @return true on success, false if @p pObj is null.
inline bool serialize(ISerializable* pObj, std::string& output)
{
    if (pObj == nullptr)
        return false;

    json::Value serializeRoot;
    pObj->serialize(serializeRoot);
    output = serializeRoot.dump(4);
    return true;
}


/// Deserializes @p pObj from a JSON string.
/// @param pObj  Object to populate; must not be null.
/// @param input JSON string to parse.
/// @return true on success, false if @p pObj is null or parsing fails.
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
