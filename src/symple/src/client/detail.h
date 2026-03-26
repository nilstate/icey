#pragma once


#include "icy/symple/client.h"

#include <unordered_set>


namespace icy {
namespace smpl {


struct Client::ClientData
{
    explicit ClientData(Options opts, uv::Loop* targetLoop)
        : options(std::move(opts))
        , loop(targetLoop)
        , reconnectTimer(targetLoop)
    {
    }

    Options options;
    uv::Loop* loop;
    http::ClientConnection::Ptr ws;
    Roster roster;
    std::string ourID;
    std::unordered_set<std::string> currentRooms;  ///< Authoritative rooms from welcome / acks.
    std::unordered_set<std::string> desiredRooms;  ///< Rooms the client wants persisted across reconnects.
    std::unordered_set<std::string> pendingJoins;  ///< Join requests sent but not yet acknowledged.
    std::unordered_set<std::string> pendingLeaves; ///< Leave requests sent but not yet acknowledged.
    int announceStatus = 0;
    Timer reconnectTimer;
    int reconnectCount = 0;
    bool wasOnline = false;
    bool closing = false;
};


} // namespace smpl
} // namespace icy
