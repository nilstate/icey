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
#include "icy/signal.h"
#include "icy/vision/event.h"
#include "icy/vision/vision.h"

#include <cstdint>
#include <string>
#include <vector>


namespace icy {
namespace vision {


struct Vision_API MotionDetectorConfig
{
    std::string source;
    std::string detectorName = "motion";
    uint32_t gridWidth = 32;
    uint32_t gridHeight = 18;
    uint32_t warmupFrames = 2;
    float threshold = 0.08f;
    int64_t minEventIntervalUsec = 500000;
};


struct Vision_API MotionDetectorStats
{
    uint64_t seen = 0;
    uint64_t emitted = 0;
    float lastScore = 0.0f;
};


class Vision_API MotionDetector
{
public:
    Signal<void(const VisionEvent&)> Event;

    explicit MotionDetector(MotionDetectorConfig config = {});

    void setConfig(MotionDetectorConfig config);
    [[nodiscard]] MotionDetectorConfig config() const;
    [[nodiscard]] MotionDetectorStats stats() const;
    void reset();

    void process(const av::PlanarVideoPacket& packet);

private:
    static MotionDetectorConfig sanitize(MotionDetectorConfig config);
    void sampleLumaGrid(const av::PlanarVideoPacket& packet, std::vector<uint8_t>& out) const;
    float diffScore(const std::vector<uint8_t>& current) const;
    void emitEvent(const av::PlanarVideoPacket& packet, float score);

    MotionDetectorConfig _config;
    uint64_t _sequence = 0;
    uint64_t _seen = 0;
    uint64_t _emitted = 0;
    uint32_t _warmedFrames = 0;
    int _width = 0;
    int _height = 0;
    int64_t _lastEventTimeUsec = 0;
    float _lastScore = 0.0f;
    std::vector<uint8_t> _previousGrid;
    std::vector<uint8_t> _currentGrid;
};


} // namespace vision
} // namespace icy


/// @}
