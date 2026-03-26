///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "callprotocol.h"
#include "icy/symple/address.h"

#include <stdexcept>


namespace icy {
namespace wrtc::callproto {

namespace {

constexpr std::string_view kCallPrefix = "call:";

bool isControlAction(std::string_view action)
{
    return action == "init" ||
           action == "accept" ||
           action == "reject" ||
           action == "hangup";
}

bool isFullAddress(const smpl::Address& address)
{
    return !address.user.empty() && !address.id.empty();
}

} // namespace


bool isFullPeerAddress(std::string_view address)
{
    smpl::Address parsed(address);
    return parsed.valid() && isFullAddress(parsed);
}


std::string subtypeFor(std::string_view action)
{
    return std::string(kCallPrefix) + std::string(action);
}


json::Value makeSympleMessage(std::string_view action,
                              std::string_view to,
                              const json::Value& data,
                              std::string_view from)
{
    if (!isFullPeerAddress(to))
        throw std::invalid_argument("Symple call peerId must be a full user|id address");
    if (!from.empty() && !isFullPeerAddress(from))
        throw std::invalid_argument("Symple call from must be a full user|id address");

    json::Value msg;
    msg["type"] = "message";
    msg["subtype"] = subtypeFor(action);
    msg["to"] = std::string(to);
    if (!from.empty())
        msg["from"] = std::string(from);
    if (!data.empty())
        msg["data"] = data;
    return msg;
}


std::optional<ParsedMessage> parseSympleMessage(const json::Value& msg)
{
    auto subtypeIt = msg.find("subtype");
    auto fromIt = msg.find("from");
    if (subtypeIt == msg.end() || fromIt == msg.end() ||
        !subtypeIt->is_string() || !fromIt->is_string()) {
        return std::nullopt;
    }

    const auto& subtype = subtypeIt->get_ref<const std::string&>();
    if (subtype.compare(0, kCallPrefix.size(), kCallPrefix) != 0)
        return std::nullopt;

    smpl::Address from(fromIt->get_ref<const std::string&>());
    if (!from.valid() || !isFullAddress(from))
        return std::nullopt;

    ParsedMessage parsed;
    parsed.peerAddress = from.toString();
    parsed.action = subtype.substr(kCallPrefix.size());

    if (parsed.action == "offer" || parsed.action == "answer") {
        auto dataIt = msg.find("data");
        if (dataIt == msg.end() || !dataIt->is_object())
            return std::nullopt;
        parsed.kind = Kind::Sdp;
        parsed.sdpType = dataIt->value("type", "");
        parsed.sdp = dataIt->value("sdp", "");
        if (parsed.sdpType.empty() || parsed.sdp.empty())
            return std::nullopt;
        return parsed;
    }

    if (parsed.action == "candidate") {
        auto dataIt = msg.find("data");
        if (dataIt == msg.end() || !dataIt->is_object())
            return std::nullopt;
        parsed.kind = Kind::Candidate;
        parsed.candidate = dataIt->value("candidate", "");
        parsed.mid = dataIt->value("sdpMid", "");
        if (parsed.candidate.empty())
            return std::nullopt;
        return parsed;
    }

    if (isControlAction(parsed.action)) {
        parsed.kind = Kind::Control;
        auto dataIt = msg.find("data");
        if (dataIt != msg.end() && dataIt->is_object())
            parsed.reason = dataIt->value("reason", "");
        return parsed;
    }

    return std::nullopt;
}


} // namespace wrtc::callproto
} // namespace icy
