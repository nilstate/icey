///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/videopacketencoder.h"

#ifdef HAVE_FFMPEG

#include "icy/logger.h"

extern "C" {
#include <libavutil/avutil.h>
}


namespace icy {
namespace av {


namespace {

int64_t microsecondsToEncoderPts(const VideoEncoder& encoder, int64_t timeUs)
{
    if (timeUs == AV_NOPTS_VALUE)
        return AV_NOPTS_VALUE;

    const auto timeBase = encoder.stream ? encoder.stream->time_base
                                         : encoder.ctx ? encoder.ctx->time_base
                                                       : AVRational{1, AV_TIME_BASE};
    return av_rescale_q(timeUs, AVRational{1, AV_TIME_BASE}, timeBase);
}

} // namespace


VideoPacketEncoder::VideoPacketEncoder(AVFormatContext* format)
    : VideoEncoder(format)
    , PacketProcessor(VideoEncoder::emitter)
{
}


VideoPacketEncoder::~VideoPacketEncoder() noexcept
{
}


void VideoPacketEncoder::process(IPacket& packet)
{
    std::lock_guard<std::mutex> guard(_mutex);

    auto* vp = dynamic_cast<VideoPacket*>(&packet);
    if (!vp) {
        emit(packet);
        return;
    }

    auto* planar = dynamic_cast<PlanarVideoPacket*>(vp);
    if (planar) {
        encode(planar->buffer, planar->linesize,
               microsecondsToEncoderPts(*this, planar->time));
    } else {
        encode(reinterpret_cast<uint8_t*>(vp->data()),
               static_cast<int>(vp->size()),
               microsecondsToEncoderPts(*this, vp->time));
    }
}


bool VideoPacketEncoder::accepts(IPacket* packet)
{
    return dynamic_cast<VideoPacket*>(packet) != nullptr;
}


void VideoPacketEncoder::onStreamStateChange(const PacketStreamState& state)
{
    std::lock_guard<std::mutex> guard(_mutex);

    switch (state.id()) {
        case PacketStreamState::Active:
            if (!_initialized) {
                LTrace("Initializing video encoder");
                VideoEncoder::create();
                _initialized = true;
            }
            break;

        case PacketStreamState::Stopping:
            if (_initialized) {
                LTrace("Flushing video encoder");
                VideoEncoder::flush();
                VideoEncoder::close();
                _initialized = false;
            }
            break;
    }
}


} // namespace av
} // namespace icy


#endif


/// @}
