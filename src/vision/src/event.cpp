///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup vision
/// @{


#include "icy/vision/event.h"


namespace icy {
namespace vision {


FrameRef makeFrameRef(const av::PlanarVideoPacket& packet, uint64_t sequence)
{
    return {
        .sequence = sequence,
        .timeUsec = packet.time,
        .width = packet.width,
        .height = packet.height,
        .pixelFmt = packet.pixelFmt,
    };
}


json::Value toJson(const Region& region)
{
    json::Value value;
    value["x"] = region.x;
    value["y"] = region.y;
    value["width"] = region.width;
    value["height"] = region.height;
    return value;
}


json::Value toJson(const FrameRef& frame)
{
    json::Value value;
    value["sequence"] = frame.sequence;
    value["time"] = frame.timeUsec;
    value["width"] = frame.width;
    value["height"] = frame.height;
    value["pixelFmt"] = frame.pixelFmt;
    return value;
}


json::Value toJson(const Detection& detection)
{
    json::Value value;
    value["label"] = detection.label;
    value["confidence"] = detection.confidence;
    value["trackId"] = detection.trackId;
    value["region"] = toJson(detection.region);
    value["data"] = detection.data;
    return value;
}


json::Value toJson(const VisionEvent& event)
{
    json::Value value;
    value["type"] = event.type;
    value["source"] = event.source;
    value["detector"] = event.detector;
    value["time"] = event.emittedAtUsec;
    value["frame"] = toJson(event.frame);
    value["detections"] = json::Value::array();
    for (const auto& detection : event.detections)
        value["detections"].push_back(toJson(detection));
    value["data"] = event.data;
    return value;
}


} // namespace vision
} // namespace icy


/// @}
