#pragma once


#include "icy/json/json.h"
#include "icy/symple/peer.h"

#include <string_view>


namespace icy::smpl::detail {


json::Value makePeerPresence(const Peer& peer,
                             std::string_view id,
                             bool online,
                             const json::Value* extra = nullptr);


} // namespace icy::smpl::detail
