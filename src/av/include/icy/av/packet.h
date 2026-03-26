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


#include "icy/av/av.h"
#include "icy/packet.h"
#include "icy/time.h"

struct AVPacket;
struct AVFrame;


namespace icy {
namespace av {


/// Timestamped media packet carrying raw audio or video data
struct MediaPacket : public RawPacket
{
    int64_t time; ///< Presentation timestamp in microseconds.

    /// Construct with a non-owning or owning mutable buffer.
    /// @param data  Pointer to the raw data buffer (may be null).
    /// @param size  Size of the buffer in bytes.
    /// @param time  Presentation timestamp in microseconds.
    MediaPacket(uint8_t* data = nullptr, size_t size = 0, int64_t time = 0)
        : RawPacket(reinterpret_cast<char*>(data), size)
        , time(time)
    {
    }

    /// Construct with const data (copied, owning).
    /// @param data  Pointer to the const raw data buffer (data is copied).
    /// @param size  Size of the buffer in bytes.
    /// @param time  Presentation timestamp in microseconds.
    MediaPacket(const uint8_t* data, size_t size, int64_t time = 0)
        : RawPacket(reinterpret_cast<const char*>(data), size)
        , time(time)
    {
    }

    /// Copy constructor.
    MediaPacket(const MediaPacket& r)
        : RawPacket(r)
        , time(r.time)
    {
    }

    virtual ~MediaPacket() = default;

    /// @return A heap-allocated copy of this packet.
    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<MediaPacket>(*this); }

    virtual const char* className() const override { return "MediaPacket"; }
};


/// Video packet for interleaved formats
struct VideoPacket : public MediaPacket
{
    int width;    ///< Frame width in pixels.
    int height;   ///< Frame height in pixels.
    bool iframe;  ///< True if this is an intra (keyframe) frame.

    /// Non-owning pointer to the encoded AVPacket from FFmpeg.
    /// Set by VideoEncoder, read by MultiplexEncoder.
    AVPacket* avpacket = nullptr;

    /// Construct a video packet with an interleaved buffer.
    /// @param data    Pointer to the raw frame buffer (may be null).
    /// @param size    Size of the buffer in bytes.
    /// @param width   Frame width in pixels.
    /// @param height  Frame height in pixels.
    /// @param time    Presentation timestamp in microseconds.
    VideoPacket(uint8_t* data = nullptr, size_t size = 0,
                int width = 0, int height = 0, int64_t time = 0)
        : MediaPacket(data, size, time)
        , width(width)
        , height(height)
        , iframe(false)
    {
    }

    /// Copy constructor. The avpacket pointer is shallow-copied (non-owning).
    VideoPacket(const VideoPacket& r)
        : MediaPacket(r)
        , width(r.width)
        , height(r.height)
        , iframe(r.iframe)
        , avpacket(r.avpacket)
    {
    }

    virtual ~VideoPacket() = default;

    /// @return A heap-allocated copy of this packet.
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

    /// Construct a planar video packet, copying the plane pointers (not the pixel data).
    /// @param data      Array of up to 4 per-plane data pointers.
    /// @param linesize  Array of per-plane byte strides.
    /// @param pixelFmt  The pixel format string (e.g. "yuv420p").
    /// @param width     Frame width in pixels.
    /// @param height    Frame height in pixels.
    /// @param time      Presentation timestamp in microseconds.
    PlanarVideoPacket(uint8_t* data[4], const int linesize[4], const std::string& pixelFmt = "",
                      int width = 0, int height = 0, int64_t time = 0);

    /// Copy constructor. Performs a deep copy of the owned buffer if owns_buffer is set.
    PlanarVideoPacket(const PlanarVideoPacket& r);
    virtual ~PlanarVideoPacket() noexcept;

    /// @return A heap-allocated deep copy of this packet.
    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<PlanarVideoPacket>(*this); }

    const char* className() const override { return "PlanarVideoPacket"; }
};


/// Audio packet for interleaved formats
struct AudioPacket : public MediaPacket
{
    size_t numSamples; ///< Number of audio samples per channel.

    /// Non-owning pointer to the encoded AVPacket from FFmpeg.
    /// Set by AudioEncoder, read by MultiplexEncoder.
    AVPacket* avpacket = nullptr;

    /// Construct an audio packet with an interleaved sample buffer.
    /// @param data        Pointer to the interleaved sample buffer (may be null).
    /// @param size        Size of the buffer in bytes.
    /// @param numSamples  Number of samples per channel.
    /// @param time        Presentation timestamp in microseconds.
    AudioPacket(uint8_t* data = nullptr, size_t size = 0,
                size_t numSamples = 0,
                int64_t time = 0)
        : MediaPacket(data, size, time)
        , numSamples(numSamples)
    {
    }

    virtual ~AudioPacket() = default;

    /// @return A heap-allocated copy of this packet.
    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<AudioPacket>(*this); }

    /// @return A pointer to the raw interleaved sample buffer.
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

    /// Construct a planar audio packet, copying the plane pointers (not the sample data).
    /// @param data        Array of up to 4 per-channel sample buffers.
    /// @param channels    Number of audio channels.
    /// @param numSamples  Number of samples per channel.
    /// @param sampleFmt   The sample format string (e.g. "fltp").
    /// @param time        Presentation timestamp in microseconds.
    PlanarAudioPacket(uint8_t* data[4], int channels = 0, size_t numSamples = 0, //, size_t size = 0
                      const std::string& sampleFmt = "", int64_t time = 0);

    /// Copy constructor. Performs a deep copy of the owned buffer if owns_buffer is set.
    PlanarAudioPacket(const PlanarAudioPacket& r);
    virtual ~PlanarAudioPacket() noexcept;

    /// @return A heap-allocated deep copy of this packet.
    virtual std::unique_ptr<IPacket> clone() const override { return std::make_unique<PlanarAudioPacket>(*this); }

    virtual const char* className() const override { return "PlanarAudioPacket"; }
};


} // namespace av
} // namespace icy


/// @}
