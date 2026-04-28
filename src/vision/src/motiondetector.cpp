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
#include <cstdint>
#include <utility>
#include <vector>


namespace icy {
namespace vision {

namespace {

struct MotionCluster
{
    int minX = 0, minY = 0;
    int maxX = 0, maxY = 0; // inclusive
    float scoreSum = 0.0f;
    int cellCount = 0;
};

// 4-connected flood fill on cells where per-cell luma diff exceeds the
// threshold. Returns one MotionCluster per connected component, with
// per-cell normalized diffs in @p outDiffs (size = gridW * gridH).
std::vector<MotionCluster> clusterHotCells(
    const std::vector<uint8_t>& current,
    const std::vector<uint8_t>& previous,
    int gridW, int gridH,
    float cellThreshold,
    std::vector<float>& outDiffs)
{
    outDiffs.assign(current.size(), 0.0f);
    for (size_t i = 0; i < current.size() && i < previous.size(); ++i) {
        outDiffs[i] = std::abs(int(current[i]) - int(previous[i])) / 255.0f;
    }

    std::vector<int> labels(current.size(), -1);
    std::vector<MotionCluster> clusters;
    std::vector<int> stack;
    stack.reserve(current.size());

    auto idx = [gridW](int x, int y) { return y * gridW + x; };

    for (int y = 0; y < gridH; ++y) {
        for (int x = 0; x < gridW; ++x) {
            const int seed = idx(x, y);
            if (labels[seed] != -1) continue;
            if (outDiffs[seed] < cellThreshold) continue;

            const int clusterId = static_cast<int>(clusters.size());
            clusters.push_back(MotionCluster{ x, y, x, y, 0.0f, 0 });
            labels[seed] = clusterId;
            stack.push_back(seed);

            while (!stack.empty()) {
                const int j = stack.back();
                stack.pop_back();
                const int jx = j % gridW;
                const int jy = j / gridW;

                MotionCluster& c = clusters[clusterId];
                c.minX = std::min(c.minX, jx);
                c.maxX = std::max(c.maxX, jx);
                c.minY = std::min(c.minY, jy);
                c.maxY = std::max(c.maxY, jy);
                c.scoreSum += outDiffs[j];
                c.cellCount++;

                static const int dx[4] = { -1, 1, 0, 0 };
                static const int dy[4] = { 0, 0, -1, 1 };
                for (int n = 0; n < 4; ++n) {
                    const int nx = jx + dx[n];
                    const int ny = jy + dy[n];
                    if (nx < 0 || nx >= gridW || ny < 0 || ny >= gridH) continue;
                    const int ni = idx(nx, ny);
                    if (labels[ni] != -1) continue;
                    if (outDiffs[ni] < cellThreshold) continue;
                    labels[ni] = clusterId;
                    stack.push_back(ni);
                }
            }
        }
    }

    return clusters;
}

} // anon namespace


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


void MotionDetector::process(const VisionFramePacket& packet)
{
    ++_seen;

    if (!packet.valid())
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
        (packet.context.ptsUsec - _lastEventTimeUsec) >= _config.minEventIntervalUsec;

    if (score >= _config.threshold && intervalReady) {
        emitEvent(packet, score);
        _lastEventTimeUsec = packet.context.ptsUsec;
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


void MotionDetector::sampleLumaGrid(const VisionFramePacket& packet,
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


void MotionDetector::emitEvent(const VisionFramePacket& packet, float score)
{
    VisionEvent event;
    event.type = "detect";
    event.source = packet.context.sourceId.empty()
        ? _config.source
        : packet.context.sourceId;
    event.detector = _config.detectorName;
    event.emittedAtUsec = packet.context.receivedAtUsec > 0
        ? packet.context.receivedAtUsec
        : packet.context.ptsUsec;
    event.frame = packet.context;
    if (event.frame.sourceId.empty())
        event.frame.sourceId = _config.source;
    if (event.frame.streamId.empty())
        event.frame.streamId = _config.streamId;
    if (event.frame.width <= 0)
        event.frame.width = packet.width;
    if (event.frame.height <= 0)
        event.frame.height = packet.height;
    if (event.frame.pixelFmt.empty())
        event.frame.pixelFmt = packet.pixelFmt;
    event.frame.keyframe = packet.iframe || event.frame.keyframe;

    // Per-cell luma diffs and connected-component clusters. Cell threshold
    // is half the global threshold; combined with the global gate above,
    // most events will produce at least one cluster.
    const float cellThreshold = std::max(_config.threshold * 0.5f, 0.02f);
    std::vector<float> cellDiffs;
    auto clusters = clusterHotCells(_currentGrid, _previousGrid,
                                    static_cast<int>(_config.gridWidth),
                                    static_cast<int>(_config.gridHeight),
                                    cellThreshold,
                                    cellDiffs);

    // Cap clusters by area. Very small isolated cells are noise; very many
    // clusters in one event swamps the overlay. Keep the top 8 by cell count.
    std::sort(clusters.begin(), clusters.end(),
              [](const MotionCluster& a, const MotionCluster& b) {
                  return a.cellCount > b.cellCount;
              });
    if (clusters.size() > 8)
        clusters.resize(8);

    const float gw = static_cast<float>(_config.gridWidth);
    const float gh = static_cast<float>(_config.gridHeight);

    if (!clusters.empty()) {
        for (const auto& c : clusters) {
            Detection d;
            d.label = "motion";
            d.confidence = c.cellCount > 0 ? c.scoreSum / c.cellCount : 0.0f;
            d.region = {
                .x = c.minX / gw,
                .y = c.minY / gh,
                .width = (c.maxX - c.minX + 1) / gw,
                .height = (c.maxY - c.minY + 1) / gh,
            };
            d.data["score"] = d.confidence;
            d.data["cells"] = c.cellCount;
            event.detections.push_back(std::move(d));
        }
    }
    else {
        // Diffuse motion that didn't form a cluster above cellThreshold.
        // Keep one full-frame detection so consumers always see something.
        Detection d;
        d.label = "motion";
        d.confidence = score;
        d.region = { .x = 0.0f, .y = 0.0f, .width = 1.0f, .height = 1.0f };
        d.data["score"] = score;
        event.detections.push_back(std::move(d));
    }

    // Per-cell normalized diff grid for heatmap rendering.
    json::Value grid = json::Value::object();
    grid["width"] = static_cast<int>(_config.gridWidth);
    grid["height"] = static_cast<int>(_config.gridHeight);
    json::Value cells = json::Value::array();
    cells.get_ref<json::Value::array_t&>().reserve(cellDiffs.size());
    for (float d : cellDiffs)
        cells.push_back(d);
    grid["cells"] = std::move(cells);

    event.data["score"] = score;
    event.data["gridWidth"] = _config.gridWidth;
    event.data["gridHeight"] = _config.gridHeight;
    event.data["grid"] = std::move(grid);

    Event.emit(event);
    ++_emitted;
}


} // namespace vision
} // namespace icy


/// @}
