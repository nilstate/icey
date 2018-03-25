///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#ifndef SCY_AV_Codec_H
#define SCY_AV_Codec_H


#include "scy/av/av.h"
#include "scy/time.h"

#include <iostream>
#include <list>
#include <string>


namespace scy {
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
    bool enabled;        ///< Weather or not the codec is available for use.

    //
    // Ctors/Dtors

    Codec();
    Codec(const std::string& name, int sampleRate = 0, int bitRate = 0,
          bool enabled = true);
    Codec(const std::string& name, const std::string& encoder,
          int sampleRate = 0, int bitRate = 0, bool enabled = true);
    /// Codec(const Codec& r);
    virtual ~Codec();

    //
    // Methods

    virtual std::string toString() const;
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


struct AV_API AudioCodec : public Codec
{
    int channels;
    std::string sampleFmt; ///< One of: u8, s16, s32, flt, dbl, u8p, s16p, s32p, fltp, dblp

    AudioCodec();
    AudioCodec(int channels,                       // = DEFAULT_AUDIO_CHANNELS
               int sampleRate,                     // = DEFAULT_AUDIO_SAMPLE_RATE
               const std::string& sampleFmt = "",  // = DEFAULT_AUDIO_SAMPLE_FMT
               int bitRate = 0);                   // = DEFAULT_AUDIO_BIT_RATE
    AudioCodec(const std::string& name,
               int channels = 0,
               int sampleRate = 0,
               int bitRate = 0,
               const std::string& sampleFmt = "");
    AudioCodec(const std::string& name,
               const std::string& encoder,
               int channels = 0,
               int sampleRate = 0,
               int bitRate = 0,
               const std::string& sampleFmt = "");
    // AudioCodec(const AudioCodec& r);
    virtual ~AudioCodec();

    virtual std::string toString() const override;
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


struct AV_API VideoCodec : public Codec
{
    int width;
    int height;
    double fps;
    std::string pixelFmt;

    VideoCodec();
    VideoCodec(int width, int height, double fps = 0.0,
               const std::string& pixelFmt = DEFAULT_VIDEO_PIXEL_FMT,
               int bitRate = 0,                  // = DEFAULT_VIDEO_BIT_RATE
               int sampleRate = 0);              // = DEFAULT_VIDEO_SAMPLE_RATE
    VideoCodec(const std::string& name, int width = 0, int height = 0, double fps = 0.0,
               int bitRate = 0,
               int sampleRate = 0,
               const std::string& pixelFmt = DEFAULT_VIDEO_PIXEL_FMT);
    VideoCodec(const std::string& name, const std::string& encoder,
               int width = 0, int height = 0, double fps = 0.0,
               int bitRate = 0,
               int sampleRatee = 0,
               const std::string& pixelFmt = DEFAULT_VIDEO_PIXEL_FMT);
    VideoCodec(const VideoCodec& r);
    virtual ~VideoCodec();

    virtual std::string toString() const override;
    virtual void print(std::ostream& ost) override;
};


typedef std::list<Codec> CodecList;
typedef std::list<Codec*> CodecPList;


// ---------------------------------------------------------------------
//
inline int64_t fpsToInterval(int fps)
{
    static const int64_t kMinimumInterval = time::kNumNanosecsPerSec / 10000;  // 10k fps.
    return fps ? time::kNumNanosecsPerSec / fps : kMinimumInterval;
}


inline int intervalToFps(int64_t interval)
{
    if (!interval) {
        return 0;
    }
    return static_cast<int>(time::kNumNanosecsPerSec / interval);
}


inline float intervalToFpsFloat(int64_t interval)
{
    if (!interval) {
        return 0.f;
    }
    return static_cast<float>(time::kNumNanosecsPerSec) / static_cast<float>(interval);
}


} // namespace av
} // namespace scy


#endif
