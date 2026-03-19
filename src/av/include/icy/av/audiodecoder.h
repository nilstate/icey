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

#include "icy/av/audiocontext.h"
#include "icy/av/packet.h"


namespace icy {
namespace av {


struct AudioDecoder : public AudioContext
{
    AudioDecoder(AVStream* stream);
    ~AudioDecoder() noexcept override;

    virtual void create() override;
    virtual void close() override;

    /// Decodes a the given input packet.
    /// Returns true an output packet was created, false otherwise.
    [[nodiscard]] bool decode(AVPacket& ipacket) override;

    /// Flushes buffered frames.
    /// This method should be called once after decoding.
    void flush() override;
};


} // namespace av
} // namespace icy


#endif


/// @\}
