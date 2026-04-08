///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup vision
/// @{


#pragma once


#include "icy/packetqueue.h"
#include "icy/vision/framepacket.h"
#include "icy/vision/vision.h"


namespace icy {
namespace vision {


/// Async queue for normalized detector-ready frames.
///
/// This is the intentional async boundary for detection work. Upstream stages
/// can stay borrowed until `FrameNormalizer` materializes an owned
/// `VisionFramePacket`, after which this queue bounds in-flight detection work
/// by dropping the oldest queued frames first.
class Vision_API DetectionQueue : public AsyncPacketQueue<VisionFramePacket>
{
public:
    explicit DetectionQueue(int maxFrames = 32)
        : AsyncPacketQueue<VisionFramePacket>(maxFrames)
    {
    }
};


} // namespace vision
} // namespace icy


/// @}
