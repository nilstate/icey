///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
///
/// @addtogroup vision
/// @{


#pragma once


#include "icy/vision/eventemitter.h"
#include "icy/vision/framepacket.h"
#include "icy/vision/vision.h"


namespace icy {
namespace vision {


/// Base interface for detectors that consume normalized vision frames.
class Vision_API Detector
{
public:
    virtual ~Detector() = default;

    VisionEventSignal Event;

    virtual void reset() = 0;
    virtual void process(const VisionFramePacket& packet) = 0;
};


} // namespace vision
} // namespace icy


/// @}
