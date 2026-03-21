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


#include "icy/base.h"

#ifdef HAVE_FFMPEG

#include "icy/av/mediacapture.h"


namespace icy {
namespace av {

/// This class implements a cross platform audio capturer.
class AV_API AudioCapture : public MediaCapture
{
public:
    using Ptr = std::shared_ptr<AudioCapture>;

    /// Construct without opening a device. Call openAudio() before start().
    AudioCapture();

    /// Construct and immediately open the given audio device using an AudioCodec params struct.
    /// @param device  The platform audio device identifier.
    /// @param params  The desired capture parameters (channels, sample rate, format).
    AudioCapture(const std::string& device, const av::AudioCodec& params);

    /// Construct and immediately open the given audio device with individual parameters.
    /// @param device      The platform audio device identifier.
    /// @param channels    The desired number of channels, or -1 for device default.
    /// @param sampleRate  The desired sample rate in Hz, or -1 for device default.
    /// @param sampleFmt   The desired sample format string, or "" for device default.
    AudioCapture(const std::string& device, int channels = -1, int sampleRate = -1,
                 const std::string& sampleFmt = "");
    virtual ~AudioCapture() noexcept;

    /// Open the given audio device using an AudioCodec params struct.
    /// @param device  The platform audio device identifier.
    /// @param params  The desired capture parameters.
    void openAudio(const std::string& device, const av::AudioCodec& params) override;

    /// Open the given audio device with individual parameters.
    /// Configures the FFmpeg input format and passes device options via AVDictionary.
    /// If the device cannot satisfy the requested parameters, resampling will be applied.
    /// @param device      The platform audio device identifier.
    /// @param channels    The desired number of channels, or -1 for device default.
    /// @param sampleRate  The desired sample rate in Hz, or -1 for device default.
    /// @param sampleFmt   The desired sample format string, or "" for device default.
    virtual void openAudio(const std::string& device, int channels = -1, int sampleRate = -1,
                           const std::string& sampleFmt = "");
};


} // namespace av
} // namespace icy


#endif


/// @\}
