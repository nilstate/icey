///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/fivetuple.h"


namespace icy {
namespace turn {


FiveTuple::FiveTuple()
    : _transport(net::UDP)
{
}


FiveTuple::FiveTuple(const net::Address& remote, const net::Address& local,
                     net::TransportType transport)
    : _remote(remote)
    , _local(local)
    , _transport(transport)
{
}


FiveTuple::FiveTuple(const FiveTuple& r)
    : _remote(r._remote)
    , _local(r._local)
    , _transport(r._transport)
{
}


bool FiveTuple::operator==(const FiveTuple& r) const
{
    return _remote == r._remote && _local == r._local && _transport == r._transport;
}


bool FiveTuple::operator<(const FiveTuple& r) const
{
    if (_remote.port() < r._remote.port())
        return true;
    if (r._remote.port() < _remote.port())
        return false;
    if (_local.port() < r._local.port())
        return true;
    if (r._local.port() < _local.port())
        return false;
    return false;
}


std::string FiveTuple::toString() const
{
    std::ostringstream ost;
    ost << "FiveTuple[" << _remote.toString() << ":" << _local.toString() << ":" << _transport << "]";
    return ost.str();
}


} // namespace turn
} // namespace icy


/// @\}
