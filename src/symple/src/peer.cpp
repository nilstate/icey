///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/peer.h"
#include "scy/logger.h"


namespace scy {
namespace smpl {


Peer::Peer()
{
    (*this)["type"] = "Peer";
}


Peer::Peer(const Peer& r)
    : json::value(r)
{
    (*this)["type"] = "Peer";
}


Peer::Peer(const json::value& r)
    : json::value(r)
{
    (*this)["type"] = "Peer";
}


Peer::~Peer()
{
}


void Peer::print(std::ostream& os) const
{
    os << dump();
}


bool Peer::valid()
{
    return find("id") != end() && find("user") != end() && find("type") != end();
}


Address Peer::address() const
{
    return Address(user(), id()); //, group()
}


std::string Peer::id() const
{
    return value("id", "");
}


std::string Peer::user() const
{
    return value("user", "");
}


std::string Peer::name() const
{
    return value("name", "");
}


// std::string Peer::group() const
// {
//     return value("group", "");
// }


std::string Peer::type() const
{
    return value("type", "");
}


std::string Peer::host() const
{
    return value("host", "");
}


void Peer::setID(const std::string& id)
{
    (*this)["id"] = id;
}


void Peer::setUser(const std::string& user)
{
    (*this)["user"] = user;
}


void Peer::setName(const std::string& name)
{
    (*this)["name"] = name;
}


// void Peer::setGroup(const std::string& group)
// {
//     (*this)["group"] = group;
// }


void Peer::setType(const std::string& type)
{
    (*this)["type"] = type;
}


void Peer::setHost(const std::string& host)
{
    (*this)["host"] = host;
}


} // namespace smpl
} // namespace scy


/// @\}
