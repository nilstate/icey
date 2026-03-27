///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup speech
/// @{


#pragma once


#include "icy/av/packet.h"
#include "icy/packetqueue.h"
#include "icy/speech/speech.h"


namespace icy {
namespace speech {


/// Async queue for decoded audio packets.
///
/// This is the explicit clone boundary for speech processing. Upstream stages
/// can stay borrowed/zero-copy until a packet enters this queue.
class Speech_API SpeechQueue : public AsyncPacketQueue<av::PlanarAudioPacket>
{
public:
    explicit SpeechQueue(int maxPackets = 64)
        : AsyncPacketQueue<av::PlanarAudioPacket>(maxPackets)
    {
    }
};


} // namespace speech
} // namespace icy


/// @}
