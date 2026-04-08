///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
///
/// @addtogroup vision
/// @{


#include "icy/vision/framenormalizer.h"

#include "icy/logger.h"

#ifdef HAVE_FFMPEG
#include "icy/av/codec.h"
#include "icy/av/ffmpeg.h"
#include "icy/av/videoconverter.h"

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/pixdesc.h>
}
#endif

#include <chrono>
#include <stdexcept>
#include <utility>


namespace icy {
namespace vision {
namespace {

int64_t steadyNowUsec()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

#ifdef HAVE_FFMPEG

av::AVFrameHolder makeFrameView(const av::PlanarVideoPacket& packet)
{
    av::AVFrameHolder frame{av_frame_alloc()};
    if (!frame)
        throw std::runtime_error("FrameNormalizer: cannot allocate frame view");

    const auto pixelFormat = av_get_pix_fmt(packet.pixelFmt.c_str());
    if (pixelFormat == AV_PIX_FMT_NONE)
        throw std::runtime_error("FrameNormalizer: unsupported input pixel format");

    frame->format = pixelFormat;
    frame->width = packet.width;
    frame->height = packet.height;
    frame->pts = packet.time;
    for (int plane = 0; plane < 4; ++plane) {
        frame->data[plane] = packet.buffer[plane];
        frame->linesize[plane] = packet.linesize[plane];
    }
    return frame;
}

#endif

} // namespace


struct FrameNormalizer::State
{
#ifdef HAVE_FFMPEG
    av::VideoConverter converter;
    bool ready = false;
#endif
};


FrameNormalizer::FrameNormalizer(FrameNormalizerConfig config)
    : PacketProcessor(emitter)
    , _config(sanitize(std::move(config)))
    , _state(std::make_unique<State>())
{
}


FrameNormalizer::~FrameNormalizer() = default;


void FrameNormalizer::setConfig(FrameNormalizerConfig config)
{
    _config = sanitize(std::move(config));
    reset();
}


FrameNormalizerConfig FrameNormalizer::config() const
{
    return _config;
}


FrameNormalizerStats FrameNormalizer::stats() const
{
    return {
        .seen = _seen.load(),
        .emitted = _emitted.load(),
        .dropped = _dropped.load(),
        .converted = _converted.load(),
    };
}


void FrameNormalizer::reset()
{
    _seen = 0;
    _emitted = 0;
    _dropped = 0;
    _converted = 0;
    _frameId = 0;
    _state = std::make_unique<State>();
}


void FrameNormalizer::process(IPacket& packet)
{
    auto* frame = dynamic_cast<av::PlanarVideoPacket*>(&packet);
    if (!frame) {
        ++_dropped;
        return;
    }

    ++_seen;
    try {
        auto normalized = normalize(*frame);
        if (!normalized.valid()) {
            ++_dropped;
            return;
        }

        ++_emitted;
        PacketProcessor::emit(normalized);
    }
    catch (const std::exception& exc) {
        ++_dropped;
        LWarn("FrameNormalizer dropped frame: ", exc.what());
    }
}


FrameNormalizerConfig FrameNormalizer::sanitize(FrameNormalizerConfig config)
{
    if (config.width < 0)
        config.width = 0;
    if (config.height < 0)
        config.height = 0;
    return config;
}


VisionFramePacket FrameNormalizer::normalize(const av::PlanarVideoPacket& packet)
{
    if (!packet.buffer[0] ||
        packet.width <= 0 ||
        packet.height <= 0 ||
        packet.pixelFmt.empty()) {
        throw std::runtime_error("invalid planar frame");
    }

    const int outputWidth = _config.width > 0 ? _config.width : packet.width;
    const int outputHeight = _config.height > 0 ? _config.height : packet.height;
    const std::string outputPixelFmt =
        _config.pixelFmt.empty() ? packet.pixelFmt : _config.pixelFmt;

    VisionFrameContext context{
        .sourceId = _config.sourceId,
        .streamId = _config.streamId,
        .frameId = ++_frameId,
        .ptsUsec = packet.time,
        .receivedAtUsec = steadyNowUsec(),
        .width = outputWidth,
        .height = outputHeight,
        .pixelFmt = outputPixelFmt,
        .keyframe = packet.iframe,
    };

#ifndef HAVE_FFMPEG
    (void)packet;
    throw std::runtime_error("FrameNormalizer requires FFmpeg");
#else
    const bool needsConversion =
        outputWidth != packet.width ||
        outputHeight != packet.height ||
        outputPixelFmt != packet.pixelFmt;

    if (!needsConversion)
        return VisionFramePacket(packet, std::move(context));

    auto& state = *_state;
    if (!state.ready ||
        state.converter.iparams.width != packet.width ||
        state.converter.iparams.height != packet.height ||
        state.converter.iparams.pixelFmt != packet.pixelFmt ||
        state.converter.oparams.width != outputWidth ||
        state.converter.oparams.height != outputHeight ||
        state.converter.oparams.pixelFmt != outputPixelFmt) {
        state.converter.close();
        state.converter.iparams = av::VideoCodec(
            packet.width, packet.height, 0.0, packet.pixelFmt);
        state.converter.oparams = av::VideoCodec(
            outputWidth, outputHeight, 0.0, outputPixelFmt);
        state.converter.create();
        state.ready = true;
    }

    AVFrame* inputFrame = packet.avframe;
    av::AVFrameHolder frameView;
    if (!inputFrame) {
        frameView = makeFrameView(packet);
        inputFrame = frameView.get();
    }

    AVFrame* converted = state.converter.convert(inputFrame);
    av::PlanarVideoPacket normalizedPacket(
        converted->data,
        converted->linesize,
        outputPixelFmt,
        outputWidth,
        outputHeight,
        packet.time);
    normalizedPacket.iframe = packet.iframe;

    ++_converted;
    return VisionFramePacket(normalizedPacket, std::move(context));
#endif
}


} // namespace vision
} // namespace icy


/// @}
