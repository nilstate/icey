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


/// Symple presence message indicating a peer's online status.
///
/// Presence messages carry peer data in the `data` field. When `probe`
/// is true the recipient should respond with their own presence.
class Symple_API Presence : public Message
{
public:
    /// Constructs a presence message with type set to "presence".
    Presence();

    /// Constructs a presence message from a JSON value.
    /// @param root JSON object to initialise from.
    Presence(const json::Value& root);

    /// Copy constructor.
    /// @param root Source presence message.
    Presence(const Presence& root);

    virtual ~Presence();

    /// Returns true if this is a presence probe request.
    /// Recipients of a probe should send back their own presence.
    bool isProbe();

    /// Sets or clears the probe flag on this presence message.
    /// @param flag True to mark this as a probe.
    void setProbe(bool flag);
};


} // namespace smpl
} // namespace icy


/// @\}
