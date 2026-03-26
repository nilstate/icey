///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
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
/// Apple CoreAudio device enumeration helpers.
namespace coreaudio {


/// Enumerate audio input and output devices using CoreAudio.
/// Populates audio capabilities (sample rates, channels).
/// @param type     The device type to enumerate (AudioInput or AudioOutput).
/// @param devices  Output vector to fill with discovered devices.
/// @return True if enumeration succeeded.
bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);


} // namespace coreaudio


//
// Apple Device Watcher
//

/// Monitors device add/remove events via AVFoundation notifications
/// and CoreAudio property listeners.
class AV_API AppleDeviceWatcher : public DeviceWatcher
{
public:
    /// @param manager  The DeviceManager whose DevicesChanged signal will be emitted on changes.
    explicit AppleDeviceWatcher(DeviceManager* manager);
    ~AppleDeviceWatcher() noexcept override;

    /// Begin monitoring for device connect/disconnect events.
    /// @return True if monitoring was successfully started.
    bool start() override;

    /// Stop monitoring and release all notification observers.
    void stop() override;

private:
    /// Internal implementation for AppleDeviceWatcher
    struct Impl;
    std::unique_ptr<Impl> _impl;
    DeviceManager* _manager;
};


} // namespace av
} // namespace icy

/// @}
