///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
///
/// @addtogroup vision
/// @{


#include "icy/vision/framepacket.h"

#include <utility>


namespace icy {
namespace vision {


#ifdef HAVE_FFMPEG

namespace {

VisionFrameContext normalizeContext(const av::PlanarVideoPacket& packet,
                                    VisionFrameContext context)
{
    context.ptsUsec = packet.time;
    context.width = packet.width;
    context.height = packet.height;
    context.pixelFmt = packet.pixelFmt;
    context.keyframe = packet.iframe || context.keyframe;
    return context;
}

} // namespace

#endif


VisionFramePacket::VisionFramePacket(const av::PlanarVideoPacket& packet,
                                     VisionFrameContext context)
#ifdef HAVE_FFMPEG
    : av::PlanarVideoPacket(const_cast<uint8_t**>(packet.buffer),
                            packet.linesize,
                            packet.pixelFmt,
                            packet.width,
                            packet.height,
                            packet.time)
    , context(normalizeContext(packet, std::move(context)))
#endif
{
#ifdef HAVE_FFMPEG
    iframe = packet.iframe;
    avframe = packet.avframe;
    flags = packet.flags;
    opaque = packet.opaque;
    info = packet.info ? packet.info->clone() : nullptr;
#endif
}


VisionFramePacket::VisionFramePacket(const VisionFramePacket& that)
#ifdef HAVE_FFMPEG
    : av::PlanarVideoPacket(that)
    , context(that.context)
#endif
{
}


} // namespace vision
} // namespace icy


/// @}
