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


#ifndef SCY_APPLE
#error "This file is only meant to be compiled for Apple targets"
#endif


namespace scy {
namespace av {
namespace coreaudio {


/// Enumerate audio input and output devices using CoreAudio.
/// Populates audio capabilities (sample rates, channels).
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
    explicit AppleDeviceWatcher(DeviceManager* manager);
    ~AppleDeviceWatcher() noexcept override;

    bool start() override;
    void stop() override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
    DeviceManager* _manager;
};


} // namespace av
} // namespace scy
