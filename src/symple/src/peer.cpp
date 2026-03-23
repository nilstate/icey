///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/peer.h"
#include "icy/logger.h"


namespace icy {
namespace smpl {


Peer::Peer()
{
    (*this)["type"] = "Peer";
}


Peer::Peer(const Peer& r)
    : json::Value(static_cast<const json::Value&>(r))
{
}


Peer& Peer::operator=(const Peer& r)
{
    if (this != &r) {
        json::Value::operator=(static_cast<const json::Value&>(r));
    }
    return *this;
}


Peer::Peer(const json::Value& r)
    : json::Value(r)
{
    if (!contains("type") || !(*this)["type"].is_string())
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


void Peer::setID(std::string_view id)
{
    (*this)["id"] = std::string(id);
}


void Peer::setUser(std::string_view user)
{
    (*this)["user"] = std::string(user);
}


void Peer::setName(std::string_view name)
{
    (*this)["name"] = std::string(name);
}


// void Peer::setGroup(const std::string& group)
// {
//     (*this)["group"] = group;
// }


void Peer::setType(std::string_view type)
{
    (*this)["type"] = std::string(type);
}


void Peer::setHost(std::string_view host)
{
    (*this)["host"] = std::string(host);
}


} // namespace smpl
} // namespace icy


/// @\}
