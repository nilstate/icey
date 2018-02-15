///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#ifndef SCY_AV_DeviceManager_H
#define SCY_AV_DeviceManager_H

#include <string>
#include <vector>

#include "scy/av/av.h"
#include "scy/signal.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef HAVE_FFMPEG_AVDEVICE
#include <libavdevice/avdevice.h>
#endif
}

#endif


namespace scy {
namespace av {


class AV_API DeviceManager;


//
// Device
//

/// Represents a system audio, video or render device.
struct AV_API Device
{
    enum Type
    {
        Unknown,
        VideoInput,
        VideoOutput,
        AudioInput,
        AudioOutput
    };

    Type type;
    std::string id;
    std::string name;
    bool isDefault;

    Device();
    Device(Type type, const std::string& id, const std::string& name, bool isDefault = false);

    void print(std::ostream& os);

    bool operator==(const Device& that) const
    {
        return type == that.type && id == that.id && name == that.name;
    }
};


enum MediaCapabilities
{
    AUDIO_RECV = 1 << 0,
    AUDIO_SEND = 1 << 1,
    VIDEO_RECV = 1 << 2,
    VIDEO_SEND = 1 << 3,
};


//
// Device Watcher
//


class AV_API DeviceWatcher
{
public:
    explicit DeviceWatcher(DeviceManager*) {}
    virtual ~DeviceWatcher() {}
    virtual bool start() { return true; }
    virtual void stop() {}
};


//
// Device Manager
//


class AV_API DeviceManager
{
public:
    DeviceManager();
    ~DeviceManager();

    /// Device enumeration
    bool getCameras(std::vector<Device>& devices);
    bool getMicrophones(std::vector<Device>& devices);
    bool getSpeakers(std::vector<Device>& devices);

    /// Default devices
    bool getDefaultMicrophone(Device& device);
    bool getDefaultSpeaker(Device& device);
    bool getDefaultCamera(Device& device);

    /// Find device by name or id
    bool findCamera(const std::string& name, Device& device);
    bool findMicrophone(const std::string& name, Device& device);
    bool findSpeaker(const std::string& name, Device& device);

    /// Base device list
    bool getDeviceList(Device::Type type, std::vector<av::Device>& devices);

    /// Capabilities
    int getCapabilities();

    void setWatcher(DeviceWatcher* watcher);
    DeviceWatcher* watcher();

    /// Print all devices to the output stream.
    void print(std::ostream& ost);

    /// Returns the default `DeviceManager` singleton.
    static DeviceManager& instance();

    /// Shuts down the `MediaFactory` and deletes the singleton instance.
    static void shutdown();

    /// Signals when a system device is connecetd or removed.
    /// This signal is emitted by the platform specific `DeviceWatcher`.
    NullSignal DevicesChanged;

#ifdef HAVE_FFMPEG
    /// Find base FFmpeg formats
    AVInputFormat* findVideoInputFormat();
    AVInputFormat* findAudioInputFormat();
#endif

protected:
    std::unique_ptr<DeviceWatcher> _watcher;
};


} // namespace av
} // namespace scy


#endif // SCY_AV_DeviceManager_H
