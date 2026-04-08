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


#include "icy/vision/eventemitter.h"
#include "icy/vision/types.h"
#include "icy/vision/vision.h"

#include <vector>


namespace icy {
namespace av {
struct PlanarVideoPacket;
}

namespace vision {


struct Vision_API VisionEvent
{
    std::string schema = "vision.event.v1";
    std::string type = "detect";
    std::string source;
    std::string detector;
    int64_t emittedAtUsec = 0;
    VisionFrameContext frame;
    std::vector<Detection> detections;
    std::vector<Track> tracks;
    json::Value data = json::Value::object();
};


[[nodiscard]] Vision_API VisionFrameContext makeVisionFrameContext(
    const av::PlanarVideoPacket& packet,
    uint64_t frameId = 0,
    const std::string& sourceId = {},
    const std::string& streamId = {},
    int64_t receivedAtUsec = 0);
[[nodiscard]] inline FrameRef makeFrameRef(const av::PlanarVideoPacket& packet,
                                           uint64_t sequence = 0,
                                           const std::string& sourceId = {},
                                           const std::string& streamId = {},
                                           int64_t receivedAtUsec = 0)
{
    return makeVisionFrameContext(packet, sequence, sourceId, streamId, receivedAtUsec);
}
[[nodiscard]] Vision_API json::Value toJson(const Region& region);
[[nodiscard]] Vision_API json::Value toJson(const VisionFrameContext& frame);
[[nodiscard]] Vision_API json::Value toJson(const Detection& detection);
[[nodiscard]] Vision_API json::Value toJson(const Track& track);
[[nodiscard]] Vision_API json::Value toJson(const VisionEvent& event);


} // namespace vision
} // namespace icy


/// @}
