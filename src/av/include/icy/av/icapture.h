///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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


/// Abstract interface for audio and video capture devices
class AV_API ICapture : public PacketSource
    , public basic::Startable
{
public:
    ICapture()
        : PacketSource(this->emitter)
    {
    }
    virtual ~ICapture() = default;

    /// Start capturing and emitting packets.
    void start() override = 0;

    /// Stop capturing and release device resources.
    void stop() override = 0;

    /// Open a media file as the capture source.
    /// @param file  Path to the media file.
    virtual void openFile(const std::string& file) {}

    /// Release the capture source and any associated resources.
    virtual void close() {}

    /// Populate @p iformat with the encoder-ready format derived from this capture source.
    /// @param iformat  Output Format struct to fill.
    virtual void getEncoderFormat(Format& iformat) = 0;

    /// Populate @p params with the encoder-ready audio codec parameters.
    /// @param params  Output AudioCodec struct to fill.
    virtual void getEncoderAudioCodec(AudioCodec& params) {}

    /// Populate @p params with the encoder-ready video codec parameters.
    /// @param params  Output VideoCodec struct to fill.
    virtual void getEncoderVideoCodec(VideoCodec& params) {}

    /// Open an audio capture device with the given parameters.
    /// @param device  The platform audio device identifier.
    /// @param params  The desired capture parameters.
    virtual void openAudio(const std::string& device, const AudioCodec& params) {}

    /// Open a video capture device with the given parameters.
    /// @param device  The platform video device identifier.
    /// @param params  The desired capture parameters.
    virtual void openVideo(const std::string& device, const VideoCodec& params) {}

    /// React to PacketStream state transitions by starting or stopping capture.
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


/// @}
