///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/collection.h"
#include "icy/symple/address.h"
#include "icy/symple/peer.h"
#include "icy/symple/symple.h"
#include <string_view>


namespace icy {
namespace smpl {


/// The Roster provides a registry for active network
/// peers indexed by session ID.
class Symple_API Roster : public LiveCollection<std::string, Peer>
{
public:
    using Manager = LiveCollection<std::string, Peer>;
    using PeerMap = Manager::Map;

public:
    Roster();
    virtual ~Roster();

    /// Returns the first peer which matches the given host address.
    Peer* getByHost(std::string_view host);

    /// Returns a copy of the peer map for thread-safe iteration.
    [[nodiscard]] PeerMap peers() const;

    virtual void print(std::ostream& os) const;

    virtual const char* className() const { return "Symple::Roster"; }
};


} // namespace smpl
} // namespace icy


/// @\}
