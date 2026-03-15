///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/command.h"
#include "scy/util.h"

#include <stdexcept>


namespace scy {
namespace smpl {


Command::Command()
{
    (*this)["type"] = "command";
}


Command::Command(const Command& root)
    : Message(root)
{
    // if (find("type") == end())
    (*this)["type"] = "command";
}


Command::Command(const json::value& root)
    : Message(root)
{
    // if (find("type") == end())
    (*this)["type"] = "command";
}


Command::~Command()
{
}


bool Command::valid() const
{
    return Message::valid() && find("node") != end();
}


std::string Command::node() const
{
    return value("node", "");
}


std::string Command::action() const
{
    return value("action", "execute");
}


void Command::setNode(const std::string& node)
{
    (*this)["node"] = node;
}


void Command::setAction(const std::string& action)
{
    (*this)["action"] = action;
}


std::string Command::param(int n) const
{
    std::vector<std::string> params = util::split(node(), ':');
    if (int(params.size()) < n)
        throw std::out_of_range("Command param index out of range: " + std::to_string(n));
    return params[n - 1].c_str();
}


std::vector<std::string> Command::params()
{
    return util::split(node(), ':');
}


bool Command::matches(const std::string& xnode) const
{
    return util::matchNodes(node(), xnode, ":");
}


} // namespace smpl
} // namespace scy


/// @\}
