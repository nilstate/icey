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
#include "icy/av/packet.h"


namespace icy {
namespace vision {


VisionFrameContext makeVisionFrameContext(const av::PlanarVideoPacket& packet,
                                          uint64_t frameId,
                                          const std::string& sourceId,
                                          const std::string& streamId,
                                          int64_t receivedAtUsec)
{
    return {
        .sourceId = sourceId,
        .streamId = streamId,
        .frameId = frameId,
        .ptsUsec = packet.time,
        .receivedAtUsec = receivedAtUsec,
        .width = packet.width,
        .height = packet.height,
        .pixelFmt = packet.pixelFmt,
        .keyframe = packet.iframe,
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


json::Value toJson(const VisionFrameContext& frame)
{
    json::Value value;
    value["sourceId"] = frame.sourceId;
    value["streamId"] = frame.streamId;
    value["frameId"] = frame.frameId;
    value["ptsUsec"] = frame.ptsUsec;
    value["receivedAtUsec"] = frame.receivedAtUsec;
    value["width"] = frame.width;
    value["height"] = frame.height;
    value["pixelFmt"] = frame.pixelFmt;
    value["keyframe"] = frame.keyframe;
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


json::Value toJson(const Track& track)
{
    json::Value value;
    value["trackId"] = track.trackId;
    value["label"] = track.label;
    value["confidence"] = track.confidence;
    value["region"] = toJson(track.region);
    value["data"] = track.data;
    return value;
}


json::Value toJson(const VisionEvent& event)
{
    json::Value value;
    value["schema"] = event.schema;
    value["type"] = event.type;
    value["source"] = event.source;
    value["detector"] = event.detector;
    value["emittedAtUsec"] = event.emittedAtUsec;
    value["frame"] = toJson(event.frame);
    value["detections"] = json::Value::array();
    for (const auto& detection : event.detections)
        value["detections"].push_back(toJson(detection));
    value["tracks"] = json::Value::array();
    for (const auto& track : event.tracks)
        value["tracks"].push_back(toJson(track));
    value["data"] = event.data;
    return value;
}


} // namespace vision
} // namespace icy


/// @}
