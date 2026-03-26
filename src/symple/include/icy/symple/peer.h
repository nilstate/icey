///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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


/// Symple peer record containing identity, presence, and custom fields.
///
/// A Peer object may also contain arbitrary application data set by
/// the client to share with other peers on the network.
/// @see Address for further methods and basic accessors.
class Symple_API Peer : public json::Value
{
public:
    /// Constructs an empty peer with type set to "Peer".
    Peer();

    /// Copy constructor; preserves type field.
    /// @param r Source peer.
    Peer(const Peer& r);

    /// Copy-assigns peer data from another peer.
    /// @param r Source peer.
    Peer& operator=(const Peer& r);

    /// Constructs a peer from a raw JSON value.
    /// @param r JSON object containing peer fields.
    Peer(const json::Value& r);

    virtual ~Peer();

    /// Returns the peer's address (user + session ID).
    /// @return Address constructed from the `user` and `id` fields.
    [[nodiscard]] Address address() const;

    /// Returns the session ID assigned by the server.
    [[nodiscard]] std::string id() const;

    /// Returns the user identifier.
    [[nodiscard]] std::string user() const;

    /// Returns the display name.
    [[nodiscard]] std::string name() const;
    // std::string group() const;

    /// Returns the peer type string (e.g. "Peer", "bot").
    [[nodiscard]] std::string type() const;

    /// Returns the host address associated with this peer.
    [[nodiscard]] std::string host() const;

    /// Sets the session ID field.
    /// @param id Session ID string.
    void setID(std::string_view id);

    /// Sets the user identifier field.
    /// @param user User identifier string.
    void setUser(std::string_view user);

    /// Sets the display name field.
    /// @param name Display name string.
    void setName(std::string_view name);
    // void setGroup(const std::string& group);

    /// Sets the peer type field.
    /// @param type Type string.
    void setType(std::string_view type);

    /// Sets the host address field.
    /// @param host Host address string.
    void setHost(std::string_view host);

    /// Updates the peer from the given data object.
    // virtual void update(const json::Value& data, bool whiny = false);

    /// Returns true if the peer has `id`, `user`, and `type` fields.
    virtual bool valid();

    /// Writes the peer's JSON representation to the given stream.
    /// @param os Output stream.
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


/// @}
