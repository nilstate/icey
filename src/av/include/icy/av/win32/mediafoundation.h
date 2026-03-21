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


#ifndef ICY_WIN
#error "This file is only meant to be compiled for Windows targets"
#endif


namespace icy {
namespace av {
namespace mediafoundation {


/// Enumerate video and audio input devices using Media Foundation.
/// Populates device capabilities (resolutions, frame rates, pixel formats).
/// @param type     The device type to enumerate (VideoInput or AudioInput).
/// @param devices  Output vector to fill with discovered devices.
/// @return True if enumeration succeeded.
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace mediafoundation


namespace wasapi {


/// Enumerate audio input and output devices using WASAPI.
/// Populates audio capabilities (sample rates, channels, formats).
/// @param type     The device type to enumerate (AudioInput or AudioOutput).
/// @param devices  Output vector to fill with discovered devices.
/// @return True if enumeration succeeded.
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace wasapi


//
// Windows Device Watcher
//

/// Monitors device add/remove events via IMMNotificationClient (audio)
/// and RegisterDeviceNotification (video).
class AV_API WindowsDeviceWatcher : public DeviceWatcher
{
public:
    /// @param manager  The DeviceManager whose DevicesChanged signal will be emitted on changes.
    explicit WindowsDeviceWatcher(DeviceManager* manager);
    ~WindowsDeviceWatcher() noexcept override;

    /// Begin monitoring via IMMNotificationClient and RegisterDeviceNotification.
    /// @return True if monitoring was successfully started.
    bool start() override;

    /// Stop monitoring and unregister all device notifications.
    void stop() override;

private:
    /// Internal implementation for WindowsDeviceWatcher
    struct Impl;
    std::unique_ptr<Impl> _impl;
    DeviceManager* _manager;
};


} // namespace av
} // namespace icy
