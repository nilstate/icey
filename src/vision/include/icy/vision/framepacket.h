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


#include "icy/av/packet.h"
#include "icy/vision/types.h"
#include "icy/vision/vision.h"


namespace icy {
namespace vision {


/// Owned detector-ready frame buffer carrying stable frame metadata.
struct Vision_API VisionFramePacket : public av::PlanarVideoPacket
{
    VisionFrameContext context;

    VisionFramePacket(const av::PlanarVideoPacket& packet,
                      VisionFrameContext context = {});
    VisionFramePacket(const VisionFramePacket& that);
    VisionFramePacket& operator=(const VisionFramePacket&) = delete;
    VisionFramePacket(VisionFramePacket&&) = delete;
    VisionFramePacket& operator=(VisionFramePacket&&) = delete;
    ~VisionFramePacket() noexcept override = default;

    std::unique_ptr<IPacket> clone() const override
    {
        return std::make_unique<VisionFramePacket>(*this);
    }

    const char* className() const override { return "VisionFramePacket"; }

    [[nodiscard]] bool valid() const
    {
        return buffer[0] && width > 0 && height > 0 && !pixelFmt.empty();
    }
};


} // namespace vision
} // namespace icy


/// @}
