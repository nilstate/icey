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


#include "icy/av/packet.h"
#include "icy/packetqueue.h"
#include "icy/vision/vision.h"


namespace icy {
namespace vision {


/// Async queue for decoded video frames.
///
/// This is the intentional clone boundary for detection work. Upstream stages
/// can keep borrowed, zero-copy semantics until a frame enters this queue.
class Vision_API DetectionQueue : public AsyncPacketQueue<av::PlanarVideoPacket>
{
public:
    explicit DetectionQueue(int maxFrames = 32)
        : AsyncPacketQueue<av::PlanarVideoPacket>(maxFrames)
    {
    }
};


} // namespace vision
} // namespace icy


/// @}
