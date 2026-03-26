#pragma once


#include "icy/json/json.h"
#include "icy/symple/peer.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>


namespace icy {
namespace smpl::proto {


struct Welcome
{
    int status = 200;
    std::string message;
    Peer peer;
    std::unordered_set<std::string> rooms;
};


[[nodiscard]] std::string addressFor(std::string_view user, std::string_view id);
[[nodiscard]] bool isReservedPresenceField(std::string_view key);
void mergePresenceExtras(json::Value& out, const json::Value* extra);

[[nodiscard]] json::Value makeError(int status, std::string_view message);
[[nodiscard]] json::Value makeClose();
[[nodiscard]] json::Value makeRoomMessage(std::string_view type, std::string_view room);
[[nodiscard]] json::Value makeWelcome(const Peer& peer,
                                      const std::unordered_set<std::string>& rooms,
                                      int status = 200,
                                      std::string_view protocol = "symple/4");
[[nodiscard]] json::Value makeAuth(std::string_view user,
                                   std::string_view name,
                                   std::string_view token,
                                   std::string_view peerType,
                                   const std::unordered_set<std::string>& rooms);
[[nodiscard]] json::Value makePresence(const Peer& peer,
                                       std::string_view id,
                                       bool online,
                                       const json::Value* extra = nullptr);
void rewritePresence(json::Value& data,
                     const Peer& peer,
                     std::string_view id,
                     bool online);

[[nodiscard]] std::optional<Welcome> parseWelcome(const json::Value& msg,
                                                  std::string* error = nullptr);
[[nodiscard]] std::optional<std::pair<int, std::string>> parseError(const json::Value& msg);
[[nodiscard]] std::optional<std::string> parseRoomMessage(const json::Value& msg,
                                                          std::string_view type);


} // namespace smpl::proto
} // namespace icy
