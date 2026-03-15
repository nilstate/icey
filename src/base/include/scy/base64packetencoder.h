///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base64.h"
#include "scy/packetstream.h"
#include "scy/signal.h"
#include <sstream>


namespace scy {


/// @addtogroup base
class /* SCY_EXTERN */ Base64PacketEncoder : public PacketProcessor
{
public:
    Base64PacketEncoder()
        : PacketProcessor(this->emitter)
    {
    }

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


} // namespace scy
