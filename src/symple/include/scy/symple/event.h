///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/symple/message.h"
#include "scy/symple/symple.h"
#include <ctime>
#include <string_view>


namespace scy {
namespace smpl {


class Symple_API Event : public Message
{
public:
    Event();
    Event(const json::Value& root);
    Event(const Event& root);
    virtual ~Event();

    [[nodiscard]] bool valid() const;

    [[nodiscard]] std::string name() const;
    // std::string message() const;
    [[nodiscard]] std::time_t time() const;

    void setName(std::string_view name);
    // void setMessage(const std::string& message);
    void setTime(std::time_t time);

    // Sets the time from a UTC time string.
    // void setTime(const std::string& time);
};


} // namespace smpl
} // namespace scy


/// @\}
