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
class Symple_API Roster : public KeyedStore<std::string, Peer>
{
public:
    Roster();
    ~Roster();

    /// Returns the first peer which matches the given host address.
    Peer* getByHost(std::string_view host);

    /// Returns a deep copy of the peer map.
    [[nodiscard]] Map peers() const;

    void print(std::ostream& os) const;

    virtual const char* className() const { return "Symple::Roster"; }

    /// Lifecycle signals for external observers (samples, UI).
    Signal<void(Peer&)> PeerAdded;
    Signal<void(const Peer&)> PeerRemoved;

protected:
    void onAdd(const std::string&, Peer* peer) override { PeerAdded.emit(*peer); }
    void onRemove(const std::string&, Peer* peer) override { PeerRemoved.emit(*peer); }
};


} // namespace smpl
} // namespace icy


/// @\}
