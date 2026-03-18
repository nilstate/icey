///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/roster.h"
#include "scy/logger.h"


namespace scy {
namespace smpl {


Roster::Roster()
{
    // LTrace("Create");
}


Roster::~Roster()
{
    // LTrace("Destroy");
}


Peer* Roster::getByHost(std::string_view host)
{
    std::shared_lock<std::shared_mutex> guard(_mutex);
    for (auto& [id, peer] : _map) {
        if (peer->host() == host)
            return peer.get();
    }
    return nullptr;
}


Roster::PeerMap Roster::peers() const
{
    std::shared_lock<std::shared_mutex> guard(_mutex);
    // Return a deep copy of the map for thread-safe iteration.
    // Each unique_ptr entry is cloned as a new Peer allocation.
    PeerMap copy;
    for (auto& [id, peer] : _map) {
        copy.emplace(id, std::make_unique<Peer>(*peer));
    }
    return copy;
}


void Roster::print(std::ostream& os) const
{
    std::shared_lock<std::shared_mutex> guard(_mutex);

    os << "Roster[";
    for (auto& [id, peer] : _map) {
        os << "\n\t" << peer.get() << ": " << id;
    }
    os << "\n]";
}


} // namespace smpl
} // namespace scy


/// @\}
