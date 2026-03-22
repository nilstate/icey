///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/roster.h"
#include "icy/logger.h"


namespace icy {
namespace smpl {


Roster::Roster()
{
}


Roster::~Roster()
{
}


Peer* Roster::getByHost(std::string_view host)
{
    for (auto& [id, peer] : _map) {
        if (peer->host() == host)
            return peer.get();
    }
    return nullptr;
}


Roster::Map Roster::peers() const
{
    Map copy;
    for (auto& [id, peer] : _map) {
        copy.emplace(id, std::make_unique<Peer>(*peer));
    }
    return copy;
}


void Roster::print(std::ostream& os) const
{
    os << "Roster[";
    for (auto& [id, peer] : _map) {
        os << "\n\t" << peer.get() << ": " << id;
    }
    os << "\n]";
}


} // namespace smpl
} // namespace icy


/// @\}
