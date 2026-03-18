///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/presence.h"
#include "scy/util.h"


namespace scy {
namespace smpl {


Presence::Presence()
{
    (*this)["type"] = "presence";
}


Presence::Presence(const Presence& root)
    : Message(root)
{
    // if (find("type") == end())
    (*this)["type"] = "presence";
}


Presence::Presence(const json::Value& root)
    : Message(root)
{
    // if (find("type") == end())
    (*this)["type"] = "presence";
}


Presence::~Presence()
{
}


bool Presence::isProbe()
{
    return value("probe", false);
}


void Presence::setProbe(bool flag)
{
    (*this)["probe"] = flag;
}


} // namespace smpl
} // namespace scy


/// @\}
