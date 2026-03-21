///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier:    LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "icy/net/socket.h"
#include "icy/turn/turn.h"

#include <sstream>


namespace icy {
namespace turn {


/// The 5-TUPLE consists of a local, a remote address, and the
/// transport protocol used by the client to communicate with the server.
///
///                                                               +---------+
///                                                               |         |
///                                                               | External|
///                                                             / | Client  |
///                                                           //  |         |
///                                                          /    |         |
///                                                        //     +---------+
///                                                       /
///                                                     //
///                     +-+                            /
///                     | |                           /
///                     | |                         //
///      +---------+    | |          +---------+   /              +---------+
///      |         |    |N|          |         | //               |         |
///      | TURN    |    | |          |         |/                 | External|
///      | Client  |----|A|----------|   TURN  |------------------| Client  |
///      |         |    | |^        ^|  Server |^                ^|         |
///      |         |    |T||        ||         ||                ||         |
///      +---------+    | ||        |+---------+|                |+---------+
///         ^           | ||        |           |                |
///         |           | ||        |           |                |
///         |           +-+|        |           |                |
///         |              |        |           |                |
///         |
///                    Internal     Internal    External         External
///     Client         Remote       Local       Local            Remote
///     Performing     Transport    Transport   Transport        Transport
///     Allocations    Address      Address     Address          Address
///
///                        |          |            |                |
///                        +-----+----+            +--------+-------+
///                              |                          |
///                              |                          |
///
///                            Internal                External
///                            5-Tuple                 5-tuple
///
class TURN_API FiveTuple
{
public:
    /// Constructs a default FiveTuple with empty addresses and UDP transport.
    FiveTuple();

    /// Constructs a FiveTuple from explicit addresses and transport.
    /// @param remote    Client's remote transport address (as seen by the server).
    /// @param local     Server's local transport address.
    /// @param transport Protocol in use (net::UDP or net::TCP).
    FiveTuple(const net::Address& remote, const net::Address& local,
              net::TransportType transport);

    /// Copy constructor.
    FiveTuple(const FiveTuple& r);

    /// @return The remote (client-side) transport address.
    [[nodiscard]] const net::Address& remote() const { return _remote; }

    /// @return The local (server-side) transport address.
    [[nodiscard]] const net::Address& local() const { return _local; }

    /// @return The transport protocol for this tuple.
    [[nodiscard]] const net::TransportType& transport() const { return _transport; }

    /// Sets the remote address.
    /// @param remote New remote address.
    void remote(const net::Address& remote) { _remote = remote; }

    /// Sets the local address.
    /// @param local New local address.
    void local(const net::Address& local) { _local = local; }

    /// Sets the transport protocol.
    /// @param transport New transport type.
    void transport(const net::TransportType& transport)
    {
        _transport = transport;
    }

    /// Equality comparison; all three components must match.
    bool operator==(const FiveTuple& r) const;

    /// Less-than ordering based on remote then local port; used as std::map key.
    bool operator<(const FiveTuple& r) const;

    /// @return A human-readable string of the form "FiveTuple[remote:local:transport]".
    [[nodiscard]] std::string toString() const;

    friend std::ostream& operator<<(std::ostream& stream,
                                    const FiveTuple& tuple)
    {
        stream << tuple.toString();
        return stream;
    }

private:
    net::Address _remote;
    net::Address _local;
    net::TransportType _transport;
};


} // namespace turn
} // namespace icy


/// @\}
