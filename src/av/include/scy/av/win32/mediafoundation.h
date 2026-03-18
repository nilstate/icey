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


namespace scy {
namespace av {
namespace mediafoundation {


/// Enumerate video and audio input devices using Media Foundation.
/// Populates device capabilities (resolutions, frame rates, pixel formats).
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace mediafoundation


namespace wasapi {


/// Enumerate audio input and output devices using WASAPI.
/// Populates audio capabilities (sample rates, channels, formats).
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
    explicit WindowsDeviceWatcher(DeviceManager* manager);
    ~WindowsDeviceWatcher() noexcept override;

    bool start() override;
    void stop() override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
    DeviceManager* _manager;
};


} // namespace av
} // namespace scy
