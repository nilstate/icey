///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{

#pragma once


#include "scy/av/devicemanager.h"
#include "scy/base.h"


#ifndef SCY_WIN
#error "This file is only meant to be compiled for Windows targets"
#endif

#ifndef HAVE_DSHOW
#define HAVE_DSHOW
#endif


namespace scy {
namespace av {
namespace dshow {


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace dshow
} // namespace av
} // namespace scy
