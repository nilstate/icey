///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/packetstream.h"

#include <list>
#include <string>

struct AVInputFormat;
struct AVDictionary;

namespace icy {
namespace av {
struct Format;
struct AudioCodec;
struct VideoCodec;


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

    virtual void openFile(const std::string& file) {}
    virtual void close() {}

    /// Sets the input format for encoding with this capture device.
    virtual void getEncoderFormat(Format& iformat) = 0;
    virtual void getEncoderAudioCodec(AudioCodec& params) {}
    virtual void getEncoderVideoCodec(VideoCodec& params) {}

    virtual void openAudio(const std::string& device, const AudioCodec& params) {}
    virtual void openVideo(const std::string& device, const VideoCodec& params) {}

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

protected:
    virtual void openStream(const std::string& filename, const AVInputFormat* inputFormat, AVDictionary** formatParams) {}
};


} // namespace av
} // namespace icy


/// @\}
