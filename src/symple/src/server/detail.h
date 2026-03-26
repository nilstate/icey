#pragma once


#include "icy/http/server.h"
#include "icy/json/json.h"
#include "icy/symple/server.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>


namespace icy {
namespace smpl {


struct Server::VirtualPeer
{
    Peer peer;
    std::unordered_set<std::string> rooms;
    std::function<void(const json::Value&)> handler;
};


class Server::PeerRegistry
{
public:
    void add(std::string id, std::unique_ptr<ServerPeer> peer);
    void addVirtual(std::string id, VirtualPeer peer);
    void bind(http::ServerConnection& conn, const std::string& id);
    void unbind(http::ServerConnection& conn);
    void erase(const std::string& id);
    void eraseVirtual(const std::string& id);
    void clear();

    [[nodiscard]] ServerPeer* find(const std::string& id);
    [[nodiscard]] const ServerPeer* find(const std::string& id) const;
    [[nodiscard]] VirtualPeer* findVirtual(const std::string& id);
    [[nodiscard]] const VirtualPeer* findVirtual(const std::string& id) const;
    [[nodiscard]] ServerPeer* findByConnection(http::ServerConnection& conn);
    [[nodiscard]] const ServerPeer* findByConnection(http::ServerConnection& conn) const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ServerPeer>>& peers() const
    {
        return _peers;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ServerPeer>> _peers;
    std::unordered_map<std::string, VirtualPeer> _virtualPeers;
    std::unordered_map<http::ServerConnection*, std::string> _connToPeer;
};


class Server::RoomIndex
{
public:
    using MemberSet = std::unordered_set<std::string>;

    void join(const std::string& room, const std::string& peerId);
    void leave(const std::string& room, const std::string& peerId);
    void leaveAll(const std::string& peerId);
    void clear();

    [[nodiscard]] const MemberSet* members(const std::string& room) const;
    [[nodiscard]] std::unordered_set<std::string> collectRecipients(
        const std::unordered_set<std::string>& rooms,
        std::string_view excludeId = {}) const;

private:
    std::unordered_map<std::string, MemberSet> _rooms;
};


struct Server::RoutingPolicy
{
    [[nodiscard]] static bool sharesAnyRoom(const std::unordered_set<std::string>& a,
                                            const std::unordered_set<std::string>& b);
    [[nodiscard]] static bool canDirectMessage(const ServerPeer& sender,
                                               const ServerPeer& recipient);
    [[nodiscard]] static bool canDirectMessage(const ServerPeer& sender,
                                               const VirtualPeer& recipient);
    [[nodiscard]] static bool canBroadcastToRoom(const ServerPeer& sender,
                                                 const std::string& room);
};


} // namespace smpl
} // namespace icy
