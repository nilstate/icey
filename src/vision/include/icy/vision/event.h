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
#include "icy/json/json.h"
#include "icy/vision/vision.h"

#include <cstdint>
#include <string>
#include <vector>


namespace icy {
namespace vision {


struct Vision_API Region
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};


struct Vision_API FrameRef
{
    uint64_t sequence = 0;
    int64_t timeUsec = 0;
    int width = 0;
    int height = 0;
    std::string pixelFmt;
};


struct Vision_API Detection
{
    std::string label;
    float confidence = 0.0f;
    int64_t trackId = -1;
    Region region;
    json::Value data = json::Value::object();
};


struct Vision_API VisionEvent
{
    std::string type = "detect";
    std::string source;
    std::string detector;
    int64_t emittedAtUsec = 0;
    FrameRef frame;
    std::vector<Detection> detections;
    json::Value data = json::Value::object();
};


[[nodiscard]] Vision_API FrameRef makeFrameRef(const av::PlanarVideoPacket& packet,
                                               uint64_t sequence = 0);
[[nodiscard]] Vision_API json::Value toJson(const Region& region);
[[nodiscard]] Vision_API json::Value toJson(const FrameRef& frame);
[[nodiscard]] Vision_API json::Value toJson(const Detection& detection);
[[nodiscard]] Vision_API json::Value toJson(const VisionEvent& event);


} // namespace vision
} // namespace icy


/// @}
