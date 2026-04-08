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


#include "icy/packetstream.h"
#include "icy/vision/framepacket.h"
#include "icy/vision/vision.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>


namespace icy {
namespace vision {


struct Vision_API FrameNormalizerConfig
{
    std::string sourceId;
    std::string streamId;
    int width = 0;
    int height = 0;
    std::string pixelFmt;
};


struct Vision_API FrameNormalizerStats
{
    uint64_t seen = 0;
    uint64_t emitted = 0;
    uint64_t dropped = 0;
    uint64_t converted = 0;
};


/// Converts sampled decoded video into owned detector-ready frames.
class Vision_API FrameNormalizer : public PacketProcessor
{
public:
    PacketSignal emitter;

    explicit FrameNormalizer(FrameNormalizerConfig config = {});
    ~FrameNormalizer() override;

    void setConfig(FrameNormalizerConfig config);
    [[nodiscard]] FrameNormalizerConfig config() const;
    [[nodiscard]] FrameNormalizerStats stats() const;
    void reset();

    void process(IPacket& packet) override;
    bool accepts(IPacket*) override
    {
        // Swallow non-video packets at the branch boundary.
        return true;
    }

private:
    struct State;

    static FrameNormalizerConfig sanitize(FrameNormalizerConfig config);

    [[nodiscard]] VisionFramePacket normalize(const av::PlanarVideoPacket& packet);

    FrameNormalizerConfig _config;
    std::atomic<uint64_t> _seen{0};
    std::atomic<uint64_t> _emitted{0};
    std::atomic<uint64_t> _dropped{0};
    std::atomic<uint64_t> _converted{0};
    std::atomic<uint64_t> _frameId{0};
    std::unique_ptr<State> _state;
};


} // namespace vision
} // namespace icy


/// @}
