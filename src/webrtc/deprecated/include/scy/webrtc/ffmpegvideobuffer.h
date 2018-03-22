///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_FFmpegVideoBuffer_H
#define SCY_WebRTC_FFmpegVideoBuffer_H


#include "scy/av/types.h"

#include "media/base/videocapturer.h"


namespace scy {
namespace wrtc {


class FFmpeg420Buffer : public webrtc::VideoFrameBuffer {
public:
    std::vector<uint8_t> ydata;
    std::vector<uint8_t> udata;
    std::vector<uint8_t> vdata;

    FFmpeg420Buffer(int width, int height,
        const uint8_t* y_plane, int y_stride,
        const uint8_t* u_plane, int u_stride,
        const uint8_t* v_plane, int v_stride);

    FFmpeg420Buffer(const av::PlanarVideoPacket& packet);

    int width() const override;
    int height() const override;

    const uint8_t* DataY() const override;
    const uint8_t* DataU() const override;
    const uint8_t* DataV() const override;

    int StrideY() const override;
    int StrideU() const override;
    int StrideV() const override;

    void* native_handle() const override;

    rtc::scoped_refptr<webrtc::VideoFrameBuffer> NativeToI420Buffer() override;

private:
    friend class rtc::RefCountedObject<FFmpeg420Buffer>;
    ~FFmpeg420Buffer() override;

    const int width_;
    const int height_;

    std::vector<uint8_t> y_data;
    std::vector<uint8_t> u_data;
    std::vector<uint8_t> v_data;

    //const uint8_t* const y_plane_;
    //const uint8_t* const u_plane_;
    //const uint8_t* const v_plane_;
    //const int y_stride_;
    //const int u_stride_;
    //const int v_stride_;
};


} } // namespace scy::wrtc


#endif


/// @\}
