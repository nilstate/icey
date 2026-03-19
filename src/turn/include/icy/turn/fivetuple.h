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
    FiveTuple();
    FiveTuple(const net::Address& remote, const net::Address& local,
              net::TransportType transport);
    FiveTuple(const FiveTuple& r);

    [[nodiscard]] const net::Address& remote() const { return _remote; }
    [[nodiscard]] const net::Address& local() const { return _local; }
    [[nodiscard]] const net::TransportType& transport() const { return _transport; }

    void remote(const net::Address& remote) { _remote = remote; }
    void local(const net::Address& local) { _local = local; }
    void transport(const net::TransportType& transport)
    {
        _transport = transport;
    }

    bool operator==(const FiveTuple& r) const;
    bool operator<(const FiveTuple& r) const;

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
