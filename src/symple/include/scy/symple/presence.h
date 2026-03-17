///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/symple/message.h"
#include "scy/symple/symple.h"


namespace scy {
namespace smpl {


class Symple_API Presence : public Message
{
public:
    Presence();
    Presence(const json::value& root);
    Presence(const Presence& root);
    virtual ~Presence();

    bool isProbe();
    void setProbe(bool flag);
};


} // namespace smpl
} // namespace scy


/// @\}
