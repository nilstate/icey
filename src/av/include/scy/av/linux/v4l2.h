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


#ifndef SCY_LINUX
#error "This file is only meant to be compiled for Linux targets"
#endif


namespace scy {
namespace av {
namespace v4l2 {


/// Enumerate video input devices using V4L2.
/// Populates device capabilities (resolutions, frame rates, pixel formats).
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace v4l2


//
// Linux Device Watcher
//

/// Monitors device add/remove events via libudev.
class AV_API LinuxDeviceWatcher : public DeviceWatcher
{
public:
    explicit LinuxDeviceWatcher(DeviceManager* manager);
    ~LinuxDeviceWatcher() noexcept override;

    bool start() override;
    void stop() override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
    DeviceManager* _manager;
};


} // namespace av
} // namespace scy
