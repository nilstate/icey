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


#include "icy/base64.h"
#include "icy/packetstream.h"
#include "icy/signal.h"
#include <sstream>


namespace icy {


/// Packet processor that Base64-encodes packet data
class /* ICY_EXTERN */ Base64PacketEncoder : public PacketProcessor
{
public:
    Base64PacketEncoder()
        : PacketProcessor(this->emitter)
    {
    }

    /// Base64-encodes the payload of an incoming `RawPacket` and emits the result.
    /// @param packet Incoming packet; must be castable to `RawPacket`.
    /// @throws std::bad_cast if the packet is not a `RawPacket`.
    virtual void process(IPacket& packet) override
    {
        auto& p = dynamic_cast<RawPacket&>(packet); // cast or throw

        base64::Encoder enc;
        std::vector<char> result(packet.size() * 2);
        size_t size = enc.encode(p.data(), p.size(), result.data());
        size += enc.finalize(result.data() + size);

        emit(result.data(), size);
    }

    PacketSignal emitter;
};


} // namespace icy

/// @}
