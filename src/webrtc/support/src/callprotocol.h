///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#pragma once


#include "icy/json/json.h"

#include <optional>
#include <string>
#include <string_view>


namespace icy {
namespace wrtc::callproto {


enum class Kind
{
    Sdp,
    Candidate,
    Control
};


struct ParsedMessage
{
    Kind kind = Kind::Control;
    std::string peerAddress;
    std::string action;
    std::string sdpType;
    std::string sdp;
    std::string candidate;
    std::string mid;
    std::string reason;
};


[[nodiscard]] bool isFullPeerAddress(std::string_view address);
[[nodiscard]] std::string subtypeFor(std::string_view action);
[[nodiscard]] json::Value makeSympleMessage(std::string_view action,
                                            std::string_view to,
                                            const json::Value& data = {},
                                            std::string_view from = {});
[[nodiscard]] std::optional<ParsedMessage> parseSympleMessage(const json::Value& msg);


} // namespace wrtc::callproto
} // namespace icy
