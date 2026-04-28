///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audiopacketencoder.h"

#ifdef HAVE_FFMPEG

#include "icy/logger.h"

extern "C" {
#include <libavutil/avutil.h>
}


namespace icy {
namespace av {


namespace {

int64_t microsecondsToEncoderPts(const AudioEncoder& encoder, int64_t timeUs)
{
    if (timeUs == AV_NOPTS_VALUE)
        return AV_NOPTS_VALUE;

    const auto timeBase = encoder.stream ? encoder.stream->time_base
                                         : encoder.ctx ? encoder.ctx->time_base
                                                       : AVRational{1, AV_TIME_BASE};
    return av_rescale_q(timeUs, AVRational{1, AV_TIME_BASE}, timeBase);
}

} // namespace


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
    if (!ap) {
        emit(packet);
        return;
    }

    auto* planar = dynamic_cast<PlanarAudioPacket*>(ap);
    if (planar) {
        (void)encode(planar->buffer, static_cast<int>(planar->numSamples),
                     microsecondsToEncoderPts(*this, planar->time));
    } else {
        (void)encode(reinterpret_cast<uint8_t*>(ap->data()),
                     static_cast<int>(ap->numSamples),
                     microsecondsToEncoderPts(*this, ap->time));
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
                // open() runs the iparams/oparams diff and creates the
                // resampler when the input format (e.g. AAC's planar fltp)
                // does not match the encoder's expected format (e.g. opus's
                // interleaved flt). Without it, planar input gets written
                // to an interleaved FIFO and channels get mis-interleaved,
                // producing garbled audio.
                AudioEncoder::open();
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
