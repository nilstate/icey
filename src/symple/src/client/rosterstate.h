#pragma once


#include "icy/json/json.h"
#include "icy/symple/peer.h"
#include "icy/symple/roster.h"

#include <string>


namespace icy::smpl::detail {


struct PresenceUpdate
{
    enum class Kind
    {
        None,
        Connected,
        Disconnected,
        Invalid
    };

    Kind kind = Kind::None;
    Peer* livePeer = nullptr;
    Peer snapshot;
    std::string error;
};


PresenceUpdate applyPresenceData(Roster& roster, const json::Value& data);


} // namespace icy::smpl::detail
