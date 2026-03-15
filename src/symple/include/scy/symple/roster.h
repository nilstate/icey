///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/collection.h"
#include "scy/symple/address.h"
#include "scy/symple/peer.h"
#include "scy/symple/symple.h"


namespace scy {
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
    Peer* getByHost(const std::string& host);

    /// Returns a copy of the peer map for thread-safe iteration.
    [[nodiscard]] virtual PeerMap peers() const;

    virtual void print(std::ostream& os) const;

    virtual const char* className() const { return "Symple::Roster"; }
};


} // namespace smpl
} // namespace scy


/// @\}
