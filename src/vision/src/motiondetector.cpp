///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup vision
/// @{


#include "icy/vision/motiondetector.h"

#include <algorithm>
#include <cmath>
#include <utility>


namespace icy {
namespace vision {


MotionDetector::MotionDetector(MotionDetectorConfig config)
    : _config(sanitize(std::move(config)))
{
}


void MotionDetector::setConfig(MotionDetectorConfig config)
{
    _config = sanitize(std::move(config));
    reset();
}


MotionDetectorConfig MotionDetector::config() const
{
    return _config;
}


MotionDetectorStats MotionDetector::stats() const
{
    return {
        .seen = _seen,
        .emitted = _emitted,
        .lastScore = _lastScore,
    };
}


void MotionDetector::reset()
{
    _sequence = 0;
    _seen = 0;
    _emitted = 0;
    _warmedFrames = 0;
    _width = 0;
    _height = 0;
    _lastEventTimeUsec = 0;
    _lastScore = 0.0f;
    _previousGrid.clear();
    _currentGrid.clear();
}


void MotionDetector::process(const av::PlanarVideoPacket& packet)
{
    ++_seen;
    ++_sequence;

    if (!packet.buffer[0] || packet.width <= 0 || packet.height <= 0)
        return;

    if (packet.width != _width || packet.height != _height) {
        _width = packet.width;
        _height = packet.height;
        _previousGrid.clear();
        _currentGrid.clear();
        _warmedFrames = 0;
    }

    sampleLumaGrid(packet, _currentGrid);
    if (_previousGrid.size() != _currentGrid.size()) {
        _previousGrid = _currentGrid;
        _warmedFrames = 1;
        return;
    }

    if (_warmedFrames < _config.warmupFrames) {
        _previousGrid = _currentGrid;
        ++_warmedFrames;
        return;
    }

    const float score = diffScore(_currentGrid);
    _lastScore = score;

    const bool intervalReady =
        _config.minEventIntervalUsec <= 0 ||
        _lastEventTimeUsec == 0 ||
        (packet.time - _lastEventTimeUsec) >= _config.minEventIntervalUsec;

    if (score >= _config.threshold && intervalReady) {
        emitEvent(packet, score);
        _lastEventTimeUsec = packet.time;
    }

    _previousGrid = _currentGrid;
}


MotionDetectorConfig MotionDetector::sanitize(MotionDetectorConfig config)
{
    if (config.gridWidth == 0)
        config.gridWidth = 1;
    if (config.gridHeight == 0)
        config.gridHeight = 1;
    if (config.threshold < 0.0f)
        config.threshold = 0.0f;
    return config;
}


void MotionDetector::sampleLumaGrid(const av::PlanarVideoPacket& packet,
                                    std::vector<uint8_t>& out) const
{
    const size_t gridSize = static_cast<size_t>(_config.gridWidth) *
                            static_cast<size_t>(_config.gridHeight);
    out.resize(gridSize);

    const auto* luma = packet.buffer[0];
    const int stride = packet.linesize[0];

    for (uint32_t gy = 0; gy < _config.gridHeight; ++gy) {
        const int y = std::min(
            static_cast<int>((static_cast<long double>(gy) * packet.height) /
                             static_cast<long double>(_config.gridHeight)),
            packet.height - 1);
        for (uint32_t gx = 0; gx < _config.gridWidth; ++gx) {
            const int x = std::min(
                static_cast<int>((static_cast<long double>(gx) * packet.width) /
                                 static_cast<long double>(_config.gridWidth)),
                packet.width - 1);
            out[static_cast<size_t>(gy) * _config.gridWidth + gx] =
                luma[static_cast<size_t>(y) * static_cast<size_t>(stride) + static_cast<size_t>(x)];
        }
    }
}


float MotionDetector::diffScore(const std::vector<uint8_t>& current) const
{
    if (_previousGrid.size() != current.size() || current.empty())
        return 0.0f;

    long double sum = 0.0L;
    for (size_t index = 0; index < current.size(); ++index) {
        sum += std::abs(static_cast<int>(current[index]) - static_cast<int>(_previousGrid[index]));
    }

    return static_cast<float>(
        sum / (static_cast<long double>(current.size()) * 255.0L));
}


void MotionDetector::emitEvent(const av::PlanarVideoPacket& packet, float score)
{
    VisionEvent event;
    event.type = "detect";
    event.source = _config.source;
    event.detector = _config.detectorName;
    event.emittedAtUsec = packet.time;
    event.frame = makeFrameRef(packet, _sequence);

    Detection detection;
    detection.label = "motion";
    detection.confidence = score;
    detection.region = {
        .x = 0.0f,
        .y = 0.0f,
        .width = 1.0f,
        .height = 1.0f,
    };
    detection.data["score"] = score;
    event.detections.push_back(std::move(detection));
    event.data["score"] = score;
    event.data["gridWidth"] = _config.gridWidth;
    event.data["gridHeight"] = _config.gridHeight;

    Event.emit(event);
    ++_emitted;
}


} // namespace vision
} // namespace icy


/// @}
