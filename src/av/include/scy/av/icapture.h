///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "scy/packetstream.h"

#include <list>


namespace scy {
namespace av {
struct Format;


class AV_API ICapture : public PacketSource
    , public basic::Startable
{
public:
    ICapture()
        : PacketSource(this->emitter)
    {
    }
    virtual ~ICapture() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    /// Sets the input format for encoding with this capture device.
    virtual void getEncoderFormat(Format& iformat) = 0;

    virtual void onStreamStateChange(const PacketStreamState& state) override
    {
        switch (state.id()) {
            case PacketStreamState::Active:
                start();
                break;
            case PacketStreamState::Stopping:
                stop();
                break;
        }
    }

    PacketSignal emitter;
};


} // namespace av
} // namespace scy


/// @\}
