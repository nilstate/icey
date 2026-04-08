///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/packet.h"

#ifdef HAVE_FFMPEG

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}




namespace icy {
namespace av {


//
// Planar Video Frame Packet
//


PlanarVideoPacket::PlanarVideoPacket(uint8_t* data[4], const int linesize[4], const std::string& pixelFmt,
                                     int width, int height, int64_t time)
    : VideoPacket(data[0], 0, width, height, time)
    , pixelFmt(pixelFmt)
{
    auto pixfmt = av_get_pix_fmt(pixelFmt.c_str());
    _size = av_image_get_buffer_size(pixfmt, width, height, 1);

    // The constructor does no copy any frame data.
    // The actual frame will not be copied unless this packet is cloned.
    for (int i = 0; i < 4; ++i) {
        this->buffer[i] = data[i];
        this->linesize[i] = linesize[i];
    }
}


PlanarVideoPacket::PlanarVideoPacket(const PlanarVideoPacket& r)
    : VideoPacket(r)
    , pixelFmt(r.pixelFmt)
    , owns_buffer(true)
    , avframe(r.avframe)
{
    if (pixelFmt.empty())
        throw std::runtime_error("PlanarVideoPacket: pixel format required to copy");
    auto pixfmt = av_get_pix_fmt(pixelFmt.c_str());

    // Allocate into this packet's buffer arrays
    int ret = av_image_alloc(buffer, linesize, width, height, pixfmt, 1);
    if (ret < 0) {
        throw std::runtime_error("PlanarVideoPacket: av_image_alloc failed");
    }
    _size = static_cast<size_t>(ret);

    // Deep copy image data from source frame
    av_image_copy(buffer, linesize,
                  const_cast<const uint8_t**>(r.buffer),
                  const_cast<const int*>(r.linesize),
                  pixfmt, width, height);
}


PlanarVideoPacket::~PlanarVideoPacket() noexcept
{
    if (owns_buffer) {
        av_freep(&buffer[0]);
    }
}


//
// Planar Audio Frame Packet
//


PlanarAudioPacket::PlanarAudioPacket(uint8_t* data[4], int channels, size_t numSamples,
                                     const std::string& sampleFmt, int64_t time)
    : AudioPacket(data[0], 0, numSamples, time)
    , channels(channels)
    , sampleFmt(sampleFmt)
{
    auto fmt = av_get_sample_fmt(sampleFmt.c_str());
    _size = av_samples_get_buffer_size(&linesize, channels, static_cast<int>(numSamples), fmt, 0);

    // The constructor does no copy any frame data.
    // The actual frame will not be copied unless this packet is cloned.
    for (int i = 0; i < 4; ++i) {
        this->buffer[i] = data[i];
    }
}


PlanarAudioPacket::PlanarAudioPacket(const PlanarAudioPacket& r)
    : AudioPacket(r)
    , channels(r.channels)
    , sampleFmt(r.sampleFmt)
    , owns_buffer(true)
{
    if (sampleFmt.empty())
        throw std::runtime_error("PlanarAudioPacket: sample format required to copy");
    auto fmt = av_get_sample_fmt(sampleFmt.c_str());

    // Allocate audio buffers into this packet's buffer array
    int ret = av_samples_alloc(buffer, &linesize, channels, static_cast<int>(numSamples), fmt, 0);
    if (ret < 0) {
        throw std::runtime_error("PlanarAudioPacket: av_samples_alloc failed");
    }

    // Deep copy audio data from source frame
    av_samples_copy(buffer, const_cast<uint8_t* const*>(r.buffer), 0, 0,
                    static_cast<int>(numSamples), channels, fmt);
}


PlanarAudioPacket::~PlanarAudioPacket() noexcept
{
    if (owns_buffer) {
        av_freep(&buffer[0]);
    }
}


} // namespace av
} // namespace icy


#else

namespace icy {
namespace av {


PlanarVideoPacket::PlanarVideoPacket(uint8_t* data[4], const int linesize[4], const std::string& pixelFmt,
                                     int width, int height, int64_t time)
    : VideoPacket(data ? data[0] : nullptr, 0, width, height, time)
    , pixelFmt(pixelFmt)
{
    for (int i = 0; i < 4; ++i) {
        buffer[i] = data ? data[i] : nullptr;
        this->linesize[i] = linesize ? linesize[i] : 0;
    }
}


PlanarVideoPacket::PlanarVideoPacket(const PlanarVideoPacket& r)
    : VideoPacket(r)
    , pixelFmt(r.pixelFmt)
    , owns_buffer(false)
    , avframe(r.avframe)
{
    for (int i = 0; i < 4; ++i) {
        buffer[i] = r.buffer[i];
        linesize[i] = r.linesize[i];
    }
}


PlanarVideoPacket::~PlanarVideoPacket() noexcept = default;


PlanarAudioPacket::PlanarAudioPacket(uint8_t* data[4], int channels, size_t numSamples,
                                     const std::string& sampleFmt, int64_t time)
    : AudioPacket(data ? data[0] : nullptr, 0, numSamples, time)
    , channels(channels)
    , sampleFmt(sampleFmt)
{
    for (int i = 0; i < 4; ++i) {
        buffer[i] = data ? data[i] : nullptr;
    }
}


PlanarAudioPacket::PlanarAudioPacket(const PlanarAudioPacket& r)
    : AudioPacket(r)
    , linesize(r.linesize)
    , channels(r.channels)
    , sampleFmt(r.sampleFmt)
    , owns_buffer(false)
{
    for (int i = 0; i < 4; ++i) {
        buffer[i] = r.buffer[i];
    }
}


PlanarAudioPacket::~PlanarAudioPacket() noexcept = default;


} // namespace av
} // namespace icy


#endif


/// @}
