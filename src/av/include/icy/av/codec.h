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


#include "icy/av/av.h"
#include "icy/time.h"

#include <iostream>
#include <list>
#include <map>
#include <string>


namespace icy {
namespace av {


/// Codec for encoding/decoding media.
struct AV_API Codec
{
    //
    // Base Codec Variables

    std::string name;    ///< The display name for this codec.
    std::string encoder; ///< The encoder name for FFmpeg.
    int sampleRate;      ///< The sampling rate or RTP clock rate.
    int bitRate;         ///< The bit rate to encode at.
    int quality;         ///< Optional quality value, variable range depending on codec.
    int compliance;      ///< FFmpeg strict_std_compliance level (default: FF_COMPLIANCE_EXPERIMENTAL).
    bool enabled;        ///< Weather or not the codec is available for use.

    /// Arbitrary encoder options passed to FFmpeg via av_opt_set().
    /// Keys are FFmpeg option names (e.g. "preset", "tune", "profile").
    /// Applied after codec-specific defaults, so they override everything.
    std::map<std::string, std::string> options;

    //
    // Ctors/Dtors

    /// Construct a disabled codec with zeroed parameters.
    Codec();

    /// Construct a codec with a display name, sample rate, bit rate, and enabled flag.
    /// @param name        The codec display name.
    /// @param sampleRate  The sampling rate or RTP clock rate in Hz.
    /// @param bitRate     The target bit rate in bits per second.
    /// @param enabled     Whether the codec is available for use.
    Codec(const std::string& name, int sampleRate = 0, int bitRate = 0,
          bool enabled = true);

    /// Construct a codec with an explicit FFmpeg encoder name.
    /// @param name        The codec display name.
    /// @param encoder     The FFmpeg encoder name (e.g. "libx264").
    /// @param sampleRate  The sampling rate or RTP clock rate in Hz.
    /// @param bitRate     The target bit rate in bits per second.
    /// @param enabled     Whether the codec is available for use.
    Codec(const std::string& name, const std::string& encoder,
          int sampleRate = 0, int bitRate = 0, bool enabled = true);
    /// Codec(const Codec& r);
    virtual ~Codec() noexcept;

    //
    // Methods

    /// Return a compact string representation of this codec.
    /// @return A string in the form "Codec[name:encoder:sampleRate:enabled]".
    virtual std::string toString() const;

    /// Print a multi-line human-readable description to the given stream.
    /// @param ost  The output stream to write to.
    virtual void print(std::ostream& ost);

    //
    // Operators
    // Codec& operator=(const Codec& r);
};


// ---------------------------------------------------------------------
//
#define DEFAULT_AUDIO_SAMPLE_RATE 44100
#define DEFAULT_AUDIO_BIT_RATE 64000
#define DEFAULT_AUDIO_CHANNELS 2
#define DEFAULT_AUDIO_SAMPLE_FMT "s16"


/// Audio codec parameters including channels, sample rate, and sample format
struct AV_API AudioCodec : public Codec
{
    int channels;
    std::string sampleFmt; ///< One of: u8, s16, s32, flt, dbl, u8p, s16p, s32p, fltp, dblp

    /// Construct a disabled audio codec with zeroed parameters.
    AudioCodec();

    /// Construct an anonymous audio codec from raw parameters.
    /// @param channels    The number of audio channels.
    /// @param sampleRate  The sample rate in Hz.
    /// @param sampleFmt   The sample format string (e.g. "s16", "fltp").
    /// @param bitRate     The target bit rate in bits per second.
    AudioCodec(int channels,                      // = DEFAULT_AUDIO_CHANNELS
               int sampleRate,                    // = DEFAULT_AUDIO_SAMPLE_RATE
               const std::string& sampleFmt = "", // = DEFAULT_AUDIO_SAMPLE_FMT
               int bitRate = 0);                  // = DEFAULT_AUDIO_BIT_RATE

    /// Construct a named audio codec.
    /// @param name        The codec display name.
    /// @param channels    The number of audio channels.
    /// @param sampleRate  The sample rate in Hz.
    /// @param bitRate     The target bit rate in bits per second.
    /// @param sampleFmt   The sample format string.
    AudioCodec(const std::string& name,
               int channels = 0,
               int sampleRate = 0,
               int bitRate = 0,
               const std::string& sampleFmt = "");

    /// Construct a named audio codec with an explicit FFmpeg encoder name.
    /// @param name        The codec display name.
    /// @param encoder     The FFmpeg encoder name (e.g. "libopus").
    /// @param channels    The number of audio channels.
    /// @param sampleRate  The sample rate in Hz.
    /// @param bitRate     The target bit rate in bits per second.
    /// @param sampleFmt   The sample format string.
    AudioCodec(const std::string& name,
               const std::string& encoder,
               int channels = 0,
               int sampleRate = 0,
               int bitRate = 0,
               const std::string& sampleFmt = "");
    // AudioCodec(const AudioCodec& r);
    virtual ~AudioCodec() noexcept;

    /// @return A string in the form "AudioCodec[name:encoder:sampleRate:bitRate:channels:sampleFmt:enabled]".
    virtual std::string toString() const override;

    /// Print a multi-line human-readable description to the given stream.
    /// @param ost  The output stream to write to.
    virtual void print(std::ostream& ost) override;

    // AudioCodec& operator==(const AudioCodec& that);
};


// ---------------------------------------------------------------------
//
// Default 128-384 kbit/s for business-oriented videoconferencing quality
// using video compression
#define DEFAULT_VIDEO_SAMPLE_RATE 384000
#define DEFAULT_VIDEO_BIT_RATE 400000
#define DEFAULT_VIDEO_PIXEL_FMT "yuv420p"


/// Video codec parameters including resolution, frame rate, and pixel format
struct AV_API VideoCodec : public Codec
{
    int width;
    int height;
    double fps;
    std::string pixelFmt;

    /// Construct a disabled video codec with zeroed parameters.
    VideoCodec();

    /// Construct an anonymous video codec from raw parameters.
    /// @param width      The frame width in pixels.
    /// @param height     The frame height in pixels.
    /// @param fps        The target frame rate in frames per second.
    /// @param pixelFmt   The pixel format string (e.g. "yuv420p").
    /// @param bitRate    The target bit rate in bits per second.
    /// @param sampleRate The RTP clock rate in Hz.
    VideoCodec(int width, int height, double fps = 0.0,
               const std::string& pixelFmt = DEFAULT_VIDEO_PIXEL_FMT,
               int bitRate = 0,     // = DEFAULT_VIDEO_BIT_RATE
               int sampleRate = 0); // = DEFAULT_VIDEO_SAMPLE_RATE

    /// Construct a named video codec.
    /// @param name       The codec display name.
    /// @param width      The frame width in pixels.
    /// @param height     The frame height in pixels.
    /// @param fps        The target frame rate in frames per second.
    /// @param bitRate    The target bit rate in bits per second.
    /// @param sampleRate The RTP clock rate in Hz.
    /// @param pixelFmt   The pixel format string.
    VideoCodec(const std::string& name, int width = 0, int height = 0, double fps = 0.0,
               int bitRate = 0,
               int sampleRate = 0,
               const std::string& pixelFmt = DEFAULT_VIDEO_PIXEL_FMT);

    /// Construct a named video codec with an explicit FFmpeg encoder name.
    /// @param name       The codec display name.
    /// @param encoder    The FFmpeg encoder name (e.g. "libx264").
    /// @param width      The frame width in pixels.
    /// @param height     The frame height in pixels.
    /// @param fps        The target frame rate in frames per second.
    /// @param bitRate    The target bit rate in bits per second.
    /// @param sampleRate The RTP clock rate in Hz.
    /// @param pixelFmt   The pixel format string.
    VideoCodec(const std::string& name, const std::string& encoder,
               int width = 0, int height = 0, double fps = 0.0,
               int bitRate = 0,
               int sampleRate = 0,
               const std::string& pixelFmt = DEFAULT_VIDEO_PIXEL_FMT);
    VideoCodec(const VideoCodec& r);
    VideoCodec& operator=(const VideoCodec&) = default;
    virtual ~VideoCodec() noexcept;

    /// @return A string in the form "VideoCodec[name:encoder:width:height:fps:pixelFmt:enabled]".
    virtual std::string toString() const override;

    /// Print a multi-line human-readable description to the given stream.
    /// @param ost  The output stream to write to.
    virtual void print(std::ostream& ost) override;
};


using CodecList = std::list<Codec>;
using CodecPList = std::list<Codec*>;


// ---------------------------------------------------------------------
//

/// Convert a frame rate to a nanosecond frame interval.
/// @param fps  The frame rate in frames per second.
/// @return The interval in nanoseconds between frames, or the minimum interval if fps is zero.
inline int64_t fpsToInterval(int fps)
{
    static const int64_t kMinimumInterval = time::kNumNanosecsPerSec / 10000; // 10k fps.
    return fps ? time::kNumNanosecsPerSec / fps : kMinimumInterval;
}


/// Convert a nanosecond frame interval to a frame rate.
/// @param interval  The nanosecond interval between frames.
/// @return The frame rate in frames per second, or zero if interval is zero.
inline int intervalToFps(int64_t interval)
{
    if (!interval) {
        return 0;
    }
    return static_cast<int>(time::kNumNanosecsPerSec / interval);
}


/// Convert a nanosecond frame interval to a floating-point frame rate.
/// @param interval  The nanosecond interval between frames.
/// @return The frame rate in frames per second as a float, or 0.0f if interval is zero.
inline float intervalToFpsFloat(int64_t interval)
{
    if (!interval) {
        return 0.f;
    }
    return static_cast<float>(time::kNumNanosecsPerSec) / static_cast<float>(interval);
}


} // namespace av
} // namespace icy
