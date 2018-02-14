///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_YuvFrameGenerator_H
#define SCY_WebRTC_YuvFrameGenerator_H


#include "scy/base.h"
#include "scy/logger.h"

#include "webrtc/base/basictypes.h"
#include "webrtc/base/constructormagic.h"


namespace scy {
namespace wrtc {


// Generates YUV420 frames with a "landscape with striped crosshair" in the
// Y-plane, plus a horizontal gradient in the U-plane and a vertical one in the
// V-plane. This makes for a nice mix of colours that is suited for both
// catching visual errors and making sure e.g. YUV->RGB/BGR conversion looks
// the same on different platforms.
// There is also a solid box bouncing around in the Y-plane, and two differently
// coloured lines bouncing horizontally and vertically in the U and V plane.
// This helps illustrating how the frame boundary goes, and can aid as a quite
// handy visual help for noticing e.g. packet loss if the frames are encoded
// and sent over the network.
class YuvFrameGenerator
{
public:
    // Constructs a frame-generator that produces frames of size |width|x|height|.
    // If |enable_barcode| is specified, barcodes can be included in the frames
    // when calling |GenerateNextFrame(uint8_t*, uint32_t)|. If |enable_barcode|
    // is |true| then |width|x|height| should be at least 160x100; otherwise this
    // constructor will abort.
    YuvFrameGenerator(int width, int height, bool enable_barcode);
    ~YuvFrameGenerator();

    // Generate the next frame. If barcode_value is not |nullptr| the value
    // referred by it will be encoded into a barcode in the frame.
    // The value should in the range: [0..9,999,999].
    // If the value exceeds this range or barcodes were not
    // requested in the constructor, this function will abort.
    void GenerateNextFrame(int32_t barcode_value);

    // Copy interleaved YUV data to the provided |frame_buffer|.
    void CopyInterleavedData(uint8_t* frame_buffer);

    const uint8_t* DataY() const;
    const uint8_t* DataU() const;
    const uint8_t* DataV() const;

    int GetFrameSize()
    {
        return frame_data_size_;
    }

    int GetHeight()
    {
        return height_;
    }

    int GetWidth()
    {
        return width_;
    }

    // Fetch the bounds of the barcode from the generator. The barcode will
    // always be at this location. This function will abort if barcodes were not
    // requested in the constructor.
    void GetBarcodeBounds(int* top, int* left, int* width, int* height);

private:
    void DrawLandscape(uint8_t* p, int w, int h);
    void DrawGradientX(uint8_t* p, int w, int h);
    void DrawGradientY(uint8_t* p, int w, int h);
    void DrawMovingLineX(uint8_t* p, int w, int h, int n);
    void DrawMovingLineY(uint8_t* p, int w, int h, int n);
    void DrawBouncingCube(uint8_t* p, int w, int h, int n);
    void DrawBarcode(uint32_t value);
    int DrawSideGuardBars(int x, int y, int height);
    int DrawMiddleGuardBars(int x, int y, int height);
    int DrawEanEncodedDigit(int digit, int x, int y, int height, bool r_code);
    void DrawBlockRectangle(uint8_t* p, int x_start, int y_start, int width, int height, int pitch, uint8_t value);

private:
    int width_;
    int height_;
    int frame_index_;
    int frame_data_size_;
    uint8_t* y_data_;
    uint8_t* u_data_;
    uint8_t* v_data_;
    int barcode_start_x_;
    int barcode_start_y_;
};


} } // namespace scy::wrtc


#endif // SCY_WebRTC_YuvFrameGenerator_H


/*
 *  Copyright (c) 2010 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
