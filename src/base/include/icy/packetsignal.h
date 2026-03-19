///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/packet.h"
#include "icy/signal.h"
#include <cstdint>


namespace icy {


/// Signal that broadcasts `IPacket` types.
using PacketSignal = Signal<void(IPacket&)>;


/// Signal slot that allows listeners to filter polymorphic `IPacket` types.
template <class Class, class RT, class PT, class IT = IPacket>
std::shared_ptr<internal::Slot<RT, IT&>>
packetSlot(Class* instance, RT (Class::*method)(PT&), int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, IT&>>(
        new PolymorphicDelegate<Class, RT, PT, IT>(instance, method), instance, id, priority);
}


} // namespace icy


/// @\}
