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


#include <string>

#include "icy/av/av.h"

#ifdef HAVE_FFMPEG

extern "C" {
#include <libavutil/audio_fifo.h>
}


namespace icy {
namespace av {


/// FIFO buffer for queuing audio samples between encoding stages
struct AV_API AudioBuffer
{
    AudioBuffer();
    ~AudioBuffer() noexcept;

    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;
    AudioBuffer(AudioBuffer&&) = delete;
    AudioBuffer& operator=(AudioBuffer&&) = delete;

    /// Allocate the audio FIFO buffer.
    ///
    /// @param sampleFmt   The sample format name (e.g. "s16", "fltp").
    /// @param channels    The number of audio channels.
    /// @param numSamples  The initial buffer capacity in samples per channel.
    void alloc(const std::string& sampleFmt, int channels, int numSamples = 1024);

    /// Discard all samples currently held in the FIFO without freeing the buffer.
    void reset();

    /// Free the underlying AVAudioFifo buffer.
    void close();

    /// Write samples into the FIFO buffer.
    ///
    /// @param samples     Array of per-channel sample buffers.
    /// @param numSamples  The number of samples per channel to write.
    void write(void** samples, int numSamples);

    /// Read samples from the FIFO buffer.
    ///
    /// @param samples     Array of per-channel sample buffers to fill.
    /// @param numSamples  The number of samples per channel to read.
    /// @return True if enough samples were available.
    bool read(void** samples, int numSamples);

    /// Return the number of samples per channel currently available in the FIFO.
    /// @return The number of available samples, or zero if the buffer is not allocated.
    int available() const;

    AVAudioFifo* fifo; ///< Underlying FFmpeg audio FIFO handle.
};


} // namespace av
} // namespace icy


#endif


/// @\}
