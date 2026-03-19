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

    AudioCapture();
    AudioCapture(const std::string& device, const av::AudioCodec& params);
    AudioCapture(const std::string& device, int channels = -1, int sampleRate = -1,
                 const std::string& sampleFmt = "");
    virtual ~AudioCapture() noexcept;

    void openAudio(const std::string& device, const av::AudioCodec& params) override;
    virtual void openAudio(const std::string& device, int channels = -1, int sampleRate = -1,
                           const std::string& sampleFmt = "");
};


} // namespace av
} // namespace icy


#endif


/// @\}
