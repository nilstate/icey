///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/symple/message.h"
#include "icy/symple/symple.h"


namespace icy {
namespace smpl {


class Symple_API Presence : public Message
{
public:
    Presence();
    Presence(const json::Value& root);
    Presence(const Presence& root);
    virtual ~Presence();

    bool isProbe();
    void setProbe(bool flag);
};


} // namespace smpl
} // namespace icy


/// @\}
