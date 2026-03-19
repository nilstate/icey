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


#include "icy/json/json.h"
#include "icy/symple/address.h"
#include "icy/symple/symple.h"
#include <string_view>


namespace icy {
namespace smpl {


/// This class represents a peer on the network.
/// A Peer object may also contain arbitrary data set by
/// the client to share with other peers on the network.
/// @see Address for further methods and basic accessors.
class Symple_API Peer : public json::Value
{
public:
    Peer();
    Peer(const Peer& r);
    Peer& operator=(const Peer& r);
    Peer(const json::Value& r);
    virtual ~Peer();

    [[nodiscard]] Address address() const;

    [[nodiscard]] std::string id() const;
    [[nodiscard]] std::string user() const;
    [[nodiscard]] std::string name() const;
    // std::string group() const;
    [[nodiscard]] std::string type() const;
    [[nodiscard]] std::string host() const;

    void setID(std::string_view id);
    void setUser(std::string_view user);
    void setName(std::string_view name);
    // void setGroup(const std::string& group);
    void setType(std::string_view type);
    void setHost(std::string_view host);

    /// Updates the peer from the given data object.
    // virtual void update(const json::Value& data, bool whiny = false);

    virtual bool valid();

    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const Peer& peer)
    {
        peer.print(os);
        return os;
    }

    virtual const char* className() const { return "smpl::Peer"; }
};


} // namespace smpl
} // namespace icy


/// @\}
