///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#ifndef SCY_AV_AudioBuffer_H
#define SCY_AV_AudioBuffer_H


#include <string>

#include "scy/av/av.h"

#ifdef HAVE_FFMPEG

extern "C" {
#include <libavutil/audio_fifo.h>
}


namespace scy {
namespace av {


struct AV_API AudioBuffer
{
    AudioBuffer();
    ~AudioBuffer();

    void alloc(const std::string& sampleFmt, int channels, int numSamples = 1024);
    void reset();
    void close();

    void write(void** samples, int numSamples);
    bool read(void** samples, int numSamples);

    int available() const;

    AVAudioFifo* fifo;
};


} // namespace av
} // namespace scy


#endif
#endif // SCY_AV_AudioBuffer_H


/// @\}
