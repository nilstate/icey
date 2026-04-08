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


#include "icy/signal.h"
#include "icy/vision/vision.h"


namespace icy {
namespace vision {


struct VisionEvent;


using VisionEventSignal = Signal<void(const VisionEvent&)>;


class Vision_API VisionEventEmitter
{
public:
    VisionEventSignal Event;

    void emit(const VisionEvent& event)
    {
        Event.emit(event);
    }
};


} // namespace vision
} // namespace icy


/// @}
