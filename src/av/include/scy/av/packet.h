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


#include "scy/av/av.h"
#include "scy/packet.h"
#include "scy/time.h"

struct AVPacket;
struct AVFrame;


namespace scy {
namespace av {


struct MediaPacket : public RawPacket
{
    int64_t time; // microseconds

    MediaPacket(uint8_t* data = nullptr, size_t size = 0, int64_t time = 0)
        : RawPacket(reinterpret_cast<char*>(data), size)
        , time(time)
    {
    }

    /// Construct with const data (copied, owning).
    MediaPacket(const uint8_t* data, size_t size, int64_t time = 0)
        : RawPacket(reinterpret_cast<const char*>(data), size)
        , time(time)
    {
    }

    MediaPacket(const MediaPacket& r)
        : RawPacket(r)
        , time(r.time)
    {
    }

    virtual ~MediaPacket() = default;

    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<MediaPacket>(*this); }

    virtual const char* className() const override { return "MediaPacket"; }
};


/// Video packet for interleaved formats
struct VideoPacket : public MediaPacket
{
    int width;
    int height;
    bool iframe;

    /// Non-owning pointer to the encoded AVPacket from FFmpeg.
    /// Set by VideoEncoder, read by MultiplexEncoder.
    AVPacket* avpacket = nullptr;

    VideoPacket(uint8_t* data = nullptr, size_t size = 0,
                int width = 0, int height = 0, int64_t time = 0)
        : MediaPacket(data, size, time)
        , width(width)
        , height(height)
        , iframe(false)
    {
    }

    VideoPacket(const VideoPacket& r)
        : MediaPacket(r)
        , width(r.width)
        , height(r.height)
        , iframe(r.iframe)
        , avpacket(r.avpacket)
    {
    }

    virtual ~VideoPacket() = default;

    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<VideoPacket>(*this); }

    virtual const char* className() const override { return "VideoPacket"; }
};


/// Video packet for planar formats.
///
/// @param data      Array of per-plane data pointers (up to 4 planes).
/// @param linesize  Array of per-plane byte strides.
/// @param pixelFmt  The pixel format name (e.g. "yuv420p").
/// @param width     The frame width in pixels.
/// @param height    The frame height in pixels.
/// @param time      The timestamp in microseconds.
struct PlanarVideoPacket : public VideoPacket
{
    uint8_t* buffer[4] = {nullptr};
    int linesize[4] = {0};
    std::string pixelFmt;
    bool owns_buffer = false;

    /// Non-owning pointer to the decoded AVFrame from FFmpeg.
    /// Set by VideoDecoder.
    AVFrame* avframe = nullptr;

    PlanarVideoPacket(uint8_t* data[4], const int linesize[4], const std::string& pixelFmt = "",
                      int width = 0, int height = 0, int64_t time = 0);
    PlanarVideoPacket(const PlanarVideoPacket& r);
    virtual ~PlanarVideoPacket() noexcept;

    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<PlanarVideoPacket>(*this); }

    virtual const char* className() const { return "PlanarVideoPacket"; }
};


/// Audio packet for interleaved formats
struct AudioPacket : public MediaPacket
{
    size_t numSamples; // number of samples per channel

    /// Non-owning pointer to the encoded AVPacket from FFmpeg.
    /// Set by AudioEncoder, read by MultiplexEncoder.
    AVPacket* avpacket = nullptr;

    AudioPacket(uint8_t* data = nullptr, size_t size = 0,
                size_t numSamples = 0,
                int64_t time = 0)
        : MediaPacket(data, size, time)
        , numSamples(numSamples)
    {
    }

    virtual ~AudioPacket() = default;

    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<AudioPacket>(*this); }

    virtual uint8_t* samples() const
    {
        return reinterpret_cast<uint8_t*>(_data);
    }

    virtual const char* className() const override { return "AudioPacket"; }
};


/// Audio packet for planar formats.
///
/// @param data        Array of per-plane sample buffers (one per channel).
/// @param channels    The number of audio channels.
/// @param numSamples  The number of samples per channel.
/// @param sampleFmt   The sample format name (e.g. "fltp").
/// @param time        The timestamp in microseconds.
struct PlanarAudioPacket : public AudioPacket
{
    uint8_t* buffer[4] = {nullptr};
    int linesize = 0;
    int channels = 0;
    std::string sampleFmt;
    bool owns_buffer = false;

    PlanarAudioPacket(uint8_t* data[4], int channels = 0, size_t numSamples = 0, //, size_t size = 0
                      const std::string& sampleFmt = "", int64_t time = 0);
    PlanarAudioPacket(const PlanarAudioPacket& r);
    virtual ~PlanarAudioPacket() noexcept;

    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<PlanarAudioPacket>(*this); }

    virtual const char* className() const override { return "PlanarAudioPacket"; }
};


} // namespace av
} // namespace scy


/// @\}
