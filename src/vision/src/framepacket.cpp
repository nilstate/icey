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


VisionFramePacket::VisionFramePacket(const av::PlanarVideoPacket& packet,
                                     VisionFrameContext context)
    : av::PlanarVideoPacket(packet)
    , context(normalizeContext(packet, std::move(context)))
{
}


VisionFramePacket::VisionFramePacket(const VisionFramePacket& that)
    : av::PlanarVideoPacket(that)
    , context(that.context)
{
}


} // namespace vision
} // namespace icy


/// @}
