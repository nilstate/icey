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

#include "icy/av/packet.h"
#include "icy/av/videocontext.h"


namespace icy {
namespace av {


struct VideoDecoder : public VideoContext
{
    VideoDecoder(AVStream* stream);
    ~VideoDecoder() noexcept override;

    virtual void create() override;
    virtual void open() override;
    virtual void close() override;

    /// Decodes a the given input packet.
    /// Input packets should use the raw `AVStream` time base. Time base
    /// conversion will happen internally.
    /// Returns true an output packet was was decoded, false otherwise.
    [[nodiscard]] bool decode(AVPacket& ipacket) override;

    /// Flushes buffered frames.
    /// This method should be called after decoding
    /// until false is returned.
    void flush() override;
};


} // namespace av
} // namespace icy


#endif


/// @\}
