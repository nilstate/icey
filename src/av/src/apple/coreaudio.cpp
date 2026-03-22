///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/apple/coreaudio.h"
#include "icy/logger.h"

#include <CoreAudio/CoreAudio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <cmath>


namespace icy {
namespace av {
namespace coreaudio {


// Helper to get a string property from an AudioObject
static std::string getStringProperty(AudioObjectID objID, const AudioObjectPropertyAddress& addr)
{
    CFStringRef cfStr = nullptr;
    UInt32 size = sizeof(CFStringRef);
    OSStatus status = AudioObjectGetPropertyData(objID, &addr, 0, nullptr, &size, &cfStr);
    if (status != noErr || !cfStr)
        return "";

    char buf[256];
    CFStringGetCString(cfStr, buf, sizeof(buf), kCFStringEncodingUTF8);
    CFRelease(cfStr);
    return std::string(buf);
}


// Check if an audio device has streams in the given direction
static bool hasStreamsInDirection(AudioObjectID deviceID, AudioObjectPropertyScope scope)
{
    AudioObjectPropertyAddress addr = {
        kAudioDevicePropertyStreams, scope, kAudioObjectPropertyElementMain
    };
    UInt32 size = 0;
    OSStatus status = AudioObjectGetPropertyDataSize(deviceID, &addr, 0, nullptr, &size);
    return (status == noErr && size > 0);
}


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices)
{
    if (type != Device::AudioInput && type != Device::AudioOutput)
        return false;

    AudioObjectPropertyScope scope = (type == Device::AudioOutput)
        ? kAudioObjectPropertyScopeOutput
        : kAudioObjectPropertyScopeInput;

    // Get default device for this direction
    AudioObjectPropertyAddress defaultAddr = {
        (type == Device::AudioOutput)
            ? kAudioHardwarePropertyDefaultOutputDevice
            : kAudioHardwarePropertyDefaultInputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };
    AudioDeviceID defaultDeviceID = kAudioObjectUnknown;
    UInt32 defaultSize = sizeof(AudioDeviceID);
    AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultAddr, 0, nullptr,
                               &defaultSize, &defaultDeviceID);

    // Get list of all audio devices
    AudioObjectPropertyAddress devicesAddr = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };
    UInt32 dataSize = 0;
    OSStatus status = AudioObjectGetPropertyDataSize(
        kAudioObjectSystemObject, &devicesAddr, 0, nullptr, &dataSize);
    if (status != noErr || dataSize == 0) {
        LWarn("CoreAudio: cannot enumerate audio devices, status=", status);
        return false;
    }

    UInt32 deviceCount = dataSize / sizeof(AudioDeviceID);
    std::vector<AudioDeviceID> deviceIDs(deviceCount);
    status = AudioObjectGetPropertyData(
        kAudioObjectSystemObject, &devicesAddr, 0, nullptr, &dataSize, deviceIDs.data());
    if (status != noErr) {
        LWarn("CoreAudio: cannot read device list, status=", status);
        return false;
    }

    for (auto deviceID : deviceIDs) {
        // Filter by direction
        if (!hasStreamsInDirection(deviceID, scope))
            continue;

        // Get device name
        AudioObjectPropertyAddress nameAddr = {
            kAudioObjectPropertyName,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMain
        };
        std::string name = getStringProperty(deviceID, nameAddr);
        if (name.empty())
            continue;

        // Get device UID
        AudioObjectPropertyAddress uidAddr = {
            kAudioDevicePropertyDeviceUID,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMain
        };
        std::string uid = getStringProperty(deviceID, uidAddr);
        if (uid.empty())
            continue;

        bool isDefault = (deviceID == defaultDeviceID);
        Device device(type, uid, name, isDefault);

        // Query available sample rates
        AudioObjectPropertyAddress ratesAddr = {
            kAudioDevicePropertyAvailableNominalSampleRates,
            scope,
            kAudioObjectPropertyElementMain
        };
        UInt32 ratesSize = 0;
        if (AudioObjectGetPropertyDataSize(deviceID, &ratesAddr, 0, nullptr, &ratesSize) == noErr
            && ratesSize > 0) {
            UInt32 rateCount = ratesSize / sizeof(AudioValueRange);
            std::vector<AudioValueRange> rates(rateCount);
            if (AudioObjectGetPropertyData(deviceID, &ratesAddr, 0, nullptr, &ratesSize, rates.data()) == noErr) {
                // Get channel count
                AudioObjectPropertyAddress fmtAddr = {
                    kAudioDevicePropertyStreamFormat,
                    scope,
                    kAudioObjectPropertyElementMain
                };
                AudioStreamBasicDescription streamFmt = {};
                UInt32 fmtSize = sizeof(streamFmt);
                int channels = 2; // fallback
                if (AudioObjectGetPropertyData(deviceID, &fmtAddr, 0, nullptr, &fmtSize, &streamFmt) == noErr) {
                    channels = static_cast<int>(streamFmt.mChannelsPerFrame);
                }

                for (auto& range : rates) {
                    // Use the max of the range as the nominal rate
                    Device::AudioCapability cap;
                    cap.sampleRate = static_cast<int>(std::round(range.mMaximum));
                    cap.channels = channels;

                    // CoreAudio typically works with 32-bit float
                    cap.sampleFormat = "f32";

                    // Avoid duplicate sample rates
                    bool dup = false;
                    for (auto& existing : device.audioCapabilities) {
                        if (existing.sampleRate == cap.sampleRate)  {
                            dup = true;
                            break;
                        }
                    }
                    if (!dup)
                        device.audioCapabilities.push_back(std::move(cap));
                }
            }
        }

        devices.push_back(std::move(device));
    }

    return !devices.empty();
}


} // namespace coreaudio


//
// Apple Device Watcher
//

/// Internal implementation for AppleDeviceWatcher
struct AppleDeviceWatcher::Impl
{
    AudioObjectPropertyAddress devicesAddr{
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };
    bool active{false};
    DeviceManager* manager{nullptr};

    static OSStatus propertyListener(
        AudioObjectID /* objectID */,
        UInt32 /* numberAddresses */,
        const AudioObjectPropertyAddress* /* addresses */,
        void* clientData)
    {
        auto* impl = static_cast<Impl*>(clientData);
        if (impl->manager)
            impl->manager->DevicesChanged.emit();
        return noErr;
    }
};


AppleDeviceWatcher::AppleDeviceWatcher(DeviceManager* manager)
    : DeviceWatcher(manager)
    , _impl(std::make_unique<Impl>())
    , _manager(manager)
{
    _impl->manager = manager;
}


AppleDeviceWatcher::~AppleDeviceWatcher() noexcept
{
    stop();
}


bool AppleDeviceWatcher::start()
{
    if (_impl->active)
        return true;

    OSStatus status = AudioObjectAddPropertyListener(
        kAudioObjectSystemObject,
        &_impl->devicesAddr,
        Impl::propertyListener,
        _impl.get());

    if (status != noErr)
        return false;

    _impl->active = true;
    return true;
}


void AppleDeviceWatcher::stop()
{
    if (!_impl->active)
        return;

    AudioObjectRemovePropertyListener(
        kAudioObjectSystemObject,
        &_impl->devicesAddr,
        Impl::propertyListener,
        _impl.get());

    _impl->active = false;
}


} // namespace av
} // namespace icy


/// @\}
