///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/symple/message.h"
#include "icy/symple/symple.h"
#include <ctime>
#include <string_view>


namespace icy {
namespace smpl {


/// Symple event message carrying a named occurrence with a timestamp.
///
/// The `name` field identifies the event. The `time` field is stored
/// as a Unix timestamp (seconds since epoch).
class Symple_API Event : public Message
{
public:
    /// Constructs an event with type "event" and time set to now.
    Event();

    /// Constructs an event from a JSON value; sets missing time to now.
    /// @param root JSON object to initialise from.
    Event(const json::Value& root);

    /// Copy constructor; preserves or sets missing time to now.
    /// @param root Source event.
    Event(const Event& root);

    virtual ~Event();

    /// Returns true if the base message is valid and the `name` field is set.
    [[nodiscard]] bool valid() const;

    /// Returns the event name string.
    [[nodiscard]] std::string name() const;
    // std::string message() const;

    /// Returns the event timestamp as a Unix time_t value.
    [[nodiscard]] std::time_t time() const;

    /// Sets the event name field.
    /// @param name Event name string.
    void setName(std::string_view name);
    // void setMessage(const std::string& message);

    /// Sets the event timestamp.
    /// @param time Unix timestamp (seconds since epoch).
    void setTime(std::time_t time);

    // Sets the time from a UTC time string.
    // void setTime(const std::string& time);
};


} // namespace smpl
} // namespace icy


/// @}
