///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audiopacketencoder.h"

#ifdef HAVE_FFMPEG

#include "icy/logger.h"


namespace icy {
namespace av {


AudioPacketEncoder::AudioPacketEncoder(AVFormatContext* format)
    : AudioEncoder(format)
    , PacketProcessor(AudioEncoder::emitter)
{
}


AudioPacketEncoder::~AudioPacketEncoder() noexcept
{
}


void AudioPacketEncoder::process(IPacket& packet)
{
    std::lock_guard<std::mutex> guard(_mutex);

    auto* ap = dynamic_cast<AudioPacket*>(&packet);
    if (!ap)
        return;

    auto* planar = dynamic_cast<PlanarAudioPacket*>(ap);
    if (planar) {
        encode(planar->buffer, static_cast<int>(planar->numSamples), planar->time);
    } else {
        encode(reinterpret_cast<uint8_t*>(ap->data()),
               static_cast<int>(ap->numSamples), ap->time);
    }
}


bool AudioPacketEncoder::accepts(IPacket* packet)
{
    return dynamic_cast<AudioPacket*>(packet) != nullptr;
}


void AudioPacketEncoder::onStreamStateChange(const PacketStreamState& state)
{
    std::lock_guard<std::mutex> guard(_mutex);

    switch (state.id()) {
        case PacketStreamState::Active:
            if (!_initialized) {
                LTrace("Initializing audio encoder");
                AudioEncoder::create();
                _initialized = true;
            }
            break;

        case PacketStreamState::Stopping:
            if (_initialized) {
                LTrace("Flushing audio encoder");
                AudioEncoder::flush();
                AudioEncoder::close();
                _initialized = false;
            }
            break;
    }
}


} // namespace av
} // namespace icy


#endif


/// @}
