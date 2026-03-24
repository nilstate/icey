///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{

#pragma once


#include "icy/base.h"

#ifdef HAVE_FFMPEG

#include "icy/av/mediacapture.h"


namespace icy {
namespace av {


/// Cross-platform video device capturer backed by FFmpeg avdevice.
class AV_API VideoCapture : public MediaCapture
{
public:
    using Ptr = std::shared_ptr<VideoCapture>;

    /// Construct without opening a device. Call openVideo() before start().
    VideoCapture();

    /// Construct and immediately open the given video device using a VideoCodec params struct.
    /// @param device  The platform video device identifier (e.g. "/dev/video0" or "0" on Windows).
    /// @param params  The desired capture parameters (width, height, fps, pixel format).
    VideoCapture(const std::string& device, const av::VideoCodec& params);

    /// Construct and immediately open the given video device with individual parameters.
    /// @param device     The platform video device identifier.
    /// @param width      The desired frame width in pixels, or 0 for device default.
    /// @param height     The desired frame height in pixels, or 0 for device default.
    /// @param framerate  The desired frame rate, or 0 for device default.
    /// @param pixelFmt   The desired pixel format string, or "" for "yuv420p".
    VideoCapture(const std::string& device, int width = 0, int height = 0,
                 double framerate = 0, const std::string& pixelFmt = "");
    virtual ~VideoCapture() noexcept;

    /// Open the given video device using a VideoCodec params struct.
    /// @param device  The platform video device identifier.
    /// @param params  The desired capture parameters.
    void openVideo(const std::string& device, const av::VideoCodec& params) override;

    /// Open the given video device with individual parameters.
    /// Configures the FFmpeg input format and passes device options via AVDictionary.
    /// If the device cannot satisfy the requested parameters, pixel format conversion
    /// and scaling will be applied by the decoder.
    /// @param device     The platform video device identifier.
    /// @param width      The desired frame width in pixels, or 0 for device default.
    /// @param height     The desired frame height in pixels, or 0 for device default.
    /// @param framerate  The desired frame rate, or 0 for device default.
    /// @param pixelFmt   The desired pixel format string, or "" for "yuv420p".
    virtual void openVideo(const std::string& device, int width = 0,
                           int height = 0, double framerate = 0,
                           const std::string& pixelFmt = "");
};


} // namespace av
} // namespace icy


#endif


/// @}
