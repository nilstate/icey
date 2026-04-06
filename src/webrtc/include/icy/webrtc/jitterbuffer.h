///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "icy/webrtc/webrtc.h"

#include <cstdint>


namespace icy {
namespace wrtc {


/// Receive-side jitter buffer behaviour for depacketized WebRTC media frames.
///
/// The jitter buffer sits after libdatachannel depacketization and before icey
/// emits encoded AudioPacket/VideoPacket objects to downstream decoders or
/// recorders. It reorders frames by RTP-derived media timestamp and delays
/// release long enough to absorb moderate network jitter.
struct WEBRTC_API JitterBufferConfig
{
    bool enabled = false;               ///< False keeps the current zero-buffer receive path.
    std::int64_t minDelayMs = 20;       ///< Base playout delay in milliseconds.
    std::int64_t maxDelayMs = 120;      ///< Upper bound for the adaptive playout delay.
    double adaptiveFactor = 1.5;        ///< Extra delay multiplier applied to observed inter-arrival variance.
    std::int64_t tickIntervalMs = 5;    ///< Poll interval for releasing buffered frames.
};


} // namespace wrtc
} // namespace icy


/// @}
