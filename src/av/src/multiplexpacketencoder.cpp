///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/multiplexpacketencoder.h"
#ifdef HAVE_FFMPEG




namespace scy {
namespace av {


MultiplexPacketEncoder::MultiplexPacketEncoder(const EncoderOptions& options)
    : MultiplexEncoder(options)
    , PacketProcessor(MultiplexEncoder::emitter)
{
}


MultiplexPacketEncoder::~MultiplexPacketEncoder() noexcept
{
}


void MultiplexPacketEncoder::process(IPacket& packet)
{
    std::lock_guard<std::mutex> guard(_mutex);

    LTrace("Processing");

    // We may be receiving either audio or video packets
    auto vPacket = dynamic_cast<VideoPacket*>(&packet);
    auto aPacket = vPacket ? nullptr : dynamic_cast<AudioPacket*>(&packet);
    if (!vPacket && !aPacket)
        throw std::invalid_argument("Unknown media packet type.");

    if (vPacket) {
        encode(*vPacket);
    } else if (aPacket) {
        encode(*aPacket);
    }
}

void MultiplexPacketEncoder::encode(VideoPacket& packet)
{
    auto planar = dynamic_cast<av::PlanarVideoPacket*>(&packet);
    if (planar) {
        encodeVideo(planar->buffer, planar->linesize, planar->width,
                    planar->height, planar->time);
    } else {
        encodeVideo(reinterpret_cast<uint8_t*>(packet.data()), static_cast<int>(packet.size()), packet.width,
                    packet.height, packet.time);
    }
}


void MultiplexPacketEncoder::encode(AudioPacket& packet)
{
    auto planar = dynamic_cast<av::PlanarAudioPacket*>(&packet);
    if (planar) {
        encodeAudio(planar->buffer, int(planar->numSamples), planar->time);
    } else {
        encodeAudio(reinterpret_cast<uint8_t*>(packet.data()), static_cast<int>(packet.numSamples), packet.time);
    }
}


bool MultiplexPacketEncoder::accepts(IPacket* packet)
{
    return dynamic_cast<av::MediaPacket*>(packet) != 0;
}


void MultiplexPacketEncoder::onStreamStateChange(const PacketStreamState& state)
{
    LTrace("On stream state change: ", state);

    std::lock_guard<std::mutex>
        guard(_mutex);

    switch (state.id()) {
        case PacketStreamState::Active:
            if (!isActive()) {
                LTrace("Initializing");
                // if (MultiplexEncoder::options().oformat.video.enabled &&
                //    MultiplexEncoder::options().oformat.audio.enabled)
                //    _muxLiveStreams = true;
                MultiplexEncoder::init();
            }
            break;

        // case PacketStreamState::Resetting:
        case PacketStreamState::Stopping:
            if (isActive()) {
                LTrace("Uninitializing");
                MultiplexEncoder::flush();
                MultiplexEncoder::uninit();
            }
            break;
            // case PacketStreamState::Stopped:
            // case PacketStreamState::Error:
            // case PacketStreamState::None:
            // case PacketStreamState::Closed:
    }

    LTrace("Stream state change: OK: ", state);
}


} // namespace av
} // namespace scy


#endif


/// @\}
