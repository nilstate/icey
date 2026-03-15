///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/address.h"
#include "scy/util.h"

#include <sstream>
#include <stdexcept>


namespace scy {
namespace smpl {


Address::Address()
{
}


Address::Address(const std::string& id)
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


bool Address::parse(const std::string& addr)
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


bool Address::operator==(const Address& r)
{
    return user == r.user && id == r.id;
}


bool Address::operator==(std::string& r)
{
    return toString() == r;
}


} // namespace smpl
} // namespace scy


/// @\}
