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


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/mediacapture.h"


namespace scy {
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

    virtual void openAudio(const std::string& device, const av::AudioCodec& params);
    virtual void openAudio(const std::string& device, int channels = -1, int sampleRate = -1,
                           const std::string& sampleFmt = "");
};


} // namespace av
} // namespace scy


#endif


/// @\}
