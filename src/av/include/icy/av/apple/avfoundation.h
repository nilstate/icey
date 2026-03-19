///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
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
namespace avfoundation {


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace avfoundation
} // namespace av
} // namespace icy
