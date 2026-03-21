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


#include "icy/av/devicemanager.h"
#include "icy/base.h"


#ifndef ICY_LINUX
#error "This file is only meant to be compiled for Linux targets"
#endif


namespace icy {
namespace av {
namespace v4l2 {


/// Enumerate video input devices using V4L2.
/// Populates device capabilities (resolutions, frame rates, pixel formats).
/// @param type     Must be Device::VideoInput.
/// @param devices  Output vector to fill with discovered devices.
/// @return True if enumeration succeeded.
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace v4l2


//
// Linux Device Watcher
//

/// Monitors device add/remove events via libudev.
class AV_API LinuxDeviceWatcher : public DeviceWatcher
{
public:
    /// @param manager  The DeviceManager whose DevicesChanged signal will be emitted on changes.
    explicit LinuxDeviceWatcher(DeviceManager* manager);
    ~LinuxDeviceWatcher() noexcept override;

    /// Begin monitoring via libudev inotify events.
    /// @return True if the udev monitor was successfully started.
    bool start() override;

    /// Stop monitoring and close the udev monitor.
    void stop() override;

private:
    /// Internal implementation for LinuxDeviceWatcher
    struct Impl;
    std::unique_ptr<Impl> _impl;
    DeviceManager* _manager;
};


} // namespace av
} // namespace icy
