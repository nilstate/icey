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


/// Creates a signal slot that filters by packet subtype `PT` before invoking `method`.
///
/// The returned slot is connected to a `PacketSignal` (which broadcasts `IPacket&`).
/// The slot performs a `dynamic_cast` on each received packet; if the cast succeeds,
/// the listener method is called with the derived type `PT`. Non-matching packets are
/// silently ignored.
///
/// @tparam Class   Listener class type.
/// @tparam RT      Return type of the listener method.
/// @tparam PT      Derived packet type the listener expects (must derive from `IT`).
/// @tparam IT      Base packet interface type; defaults to `IPacket`.
/// @param instance Pointer to the listener object.
/// @param method   Member function pointer on `Class` accepting a `PT&`.
/// @param id       Optional slot identifier; -1 for automatic assignment.
/// @param priority Optional slot priority; higher values run first.
/// @return A shared slot suitable for connecting to a `PacketSignal`.
template <class Class, class RT, class PT, class IT = IPacket>
std::shared_ptr<internal::Slot<RT, IT&>>
packetSlot(Class* instance, RT (Class::*method)(PT&), int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, IT&>>(
        new PolymorphicDelegate<Class, RT, PT, IT>(instance, method), instance, id, priority);
}


} // namespace icy


/// @\}
