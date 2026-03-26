///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/av/devicemanager.h"
#include "icy/base.h"


#ifndef ICY_APPLE
#error "This file is only meant to be compiled for Apple targets"
#endif


namespace icy {
namespace av {
/// Apple AVFoundation device enumeration helpers.
namespace avfoundation {


/// Enumerate video input devices using AVFoundation.
/// Populates device capabilities (resolutions, frame rates, pixel formats).
/// @param type     Must be Device::VideoInput.
/// @param devices  Output vector to fill with discovered devices.
/// @return True if enumeration succeeded.
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace avfoundation
} // namespace av
} // namespace icy

/// @}
