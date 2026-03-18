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

#include "scy/av/audiocontext.h"
#include "scy/av/packet.h"


namespace scy {
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
} // namespace scy


#endif


/// @\}
