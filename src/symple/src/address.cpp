///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/address.h"
#include "icy/util.h"

#include <sstream>
#include <stdexcept>


namespace icy {
namespace smpl {


Address::Address()
{
}


Address::Address(std::string_view id)
{
    parse(id);
}


Address::Address(const std::string& user, const std::string& id)
    : user(user)
    , id(id)
{
}


Address::~Address()
{
}


bool Address::parse(std::string_view addr)
{
    if (addr.empty())
        return false;

    std::vector<std::string> params = util::split(addr, '|');
    if (params.empty())
        return false;
    if (params.size() > 0)
        user = params[0];
    if (params.size() > 1)
        id = params[1];

    return valid();
}

bool Address::valid() const
{
    return !user.empty() || !id.empty();
}


void Address::print(std::ostream& os) const
{
    if (!user.empty())
        os << user;
    if (!id.empty()) {
        os << "|"; // always add slash to identify ID only addresses
        os << id;
    }
}


std::string Address::toString() const
{
    std::ostringstream os;
    print(os);
    return os.str();
}


bool Address::operator==(const Address& r) const
{
    return user == r.user && id == r.id;
}


bool Address::operator==(const std::string& r) const
{
    auto pos = r.find('|');
    if (pos == std::string::npos)
        return id.empty() && user == r;
    return std::string_view(r).substr(0, pos) == user &&
           std::string_view(r).substr(pos + 1) == id;
}


} // namespace smpl
} // namespace icy


/// @\}
