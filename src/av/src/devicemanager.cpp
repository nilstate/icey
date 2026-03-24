///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/devicemanager.h"
#include "icy/logger.h"


#if defined(ICY_WIN)
#include "icy/av/win32/mediafoundation.h"
#define ICY_VIDEO_INPUTS \
    {                    \
        "dshow"}
#define ICY_VIDEO_OUTPUTS \
    {                     \
    }
#define ICY_SCREEN_INPUTS \
    {                     \
        "gdigrab"}
#define ICY_AUDIO_INPUTS \
    {                    \
        "dshow"}
#define ICY_AUDIO_OUTPUTS \
    {                     \
        "dshow"}
#elif defined(ICY_APPLE)
#include "icy/av/apple/avfoundation.h"
#include "icy/av/apple/coreaudio.h"
#define ICY_VIDEO_INPUTS \
    {                    \
        "avfoundation"}
#define ICY_VIDEO_OUTPUTS \
    {                     \
    }
#define ICY_SCREEN_INPUTS \
    {                     \
        "avfoundation"}
#define ICY_AUDIO_INPUTS \
    {                    \
        "avfoundation"}
#define ICY_AUDIO_OUTPUTS \
    {                     \
        "avfoundation"}
#elif defined(ICY_LINUX)
#include "icy/av/linux/v4l2.h"
#define ICY_VIDEO_INPUTS \
    {                    \
        "v4l2"}
#define ICY_VIDEO_OUTPUTS \
    {                     \
    }
#define ICY_SCREEN_INPUTS \
    {                     \
        "pipewire", "x11grab"}
#define ICY_AUDIO_INPUTS \
    {                    \
        "pipewire", "pulse", "alsa"}
#define ICY_AUDIO_OUTPUTS \
    {                     \
        "pipewire", "pulse", "alsa"}
#endif




namespace icy {
namespace av {


//
// Device
//


Device::Device()
    : type(Unknown)
{
}


Device::Device(Type type, const std::string& id, const std::string& name, bool isDefault)
    : type(type)
    , id(id)
    , name(name)
    , isDefault(isDefault)
{
}


void Device::print(std::ostream& os) const
{
    os << "Device[" << type << ":" << name << ":" << id << ":" << isDefault << "]";
    for (const auto& cap : videoCapabilities) {
        os << "\n\t  " << cap.width << "x" << cap.height
           << " @ " << cap.minFps << "-" << cap.maxFps << " fps"
           << " (" << cap.pixelFormat << ")";
    }
    for (const auto& cap : audioCapabilities) {
        os << "\n\t  " << cap.sampleRate << " Hz, "
           << cap.channels << " ch"
           << " (" << cap.sampleFormat << ")";
    }
}


//
// Helpers
//


namespace internal {


#ifdef HAVE_FFMPEG

// Find an output format by name using the iterate API
const AVOutputFormat* findOutputFormat(const std::string& name)
{
    const AVOutputFormat* oformat = nullptr;
    void* opaque = nullptr;
    while ((oformat = av_muxer_iterate(&opaque))) {
        if (name == oformat->name)
            return oformat;
    }
    return nullptr;
}


const AVOutputFormat* findDefaultOutputFormat(const std::vector<std::string>& outputs)
{
    for (auto& output : outputs) {
        auto* oformat = findOutputFormat(output.c_str());
        if (oformat)
            return oformat;
    }
    LWarn("No output format found");
    return nullptr;
}


const AVInputFormat* findDefaultInputFormat(const std::vector<std::string>& inputs)
{
    for (auto& input : inputs) {
        auto* iformat = av_find_input_format(input.c_str());
        if (iformat)
            return iformat;
    }
    LWarn("No input format found");
    return nullptr;
}


bool getInputDeviceList(const std::vector<std::string>& inputs,
                        Device::Type type, std::vector<av::Device>& devices)
{
#ifndef HAVE_FFMPEG_AVDEVICE
    SWarn << "HAVE_FFMPEG_AVDEVICE not defined, cannot list input devices";
    return false;
#else
    const AVInputFormat* iformat = findDefaultInputFormat(inputs);
    if (!iformat)
        return false;

    AVDeviceInfoList* devlist = nullptr;
    int error = avdevice_list_input_sources(iformat, nullptr, nullptr, &devlist);
    if (error < 0 || !devlist) {
        LWarn("Cannot list input devices for '", iformat->name, "': ", averror(error));
        if (devlist)
            avdevice_free_list_devices(&devlist);
        return false;
    }

    devices.clear();
    for (int i = 0; i < devlist->nb_devices; i++) {
        auto dev = devlist->devices[i];
        devices.push_back(
            av::Device(type, dev->device_name, dev->device_description,
                       i == devlist->default_device));
    }

    avdevice_free_list_devices(&devlist);
    return !devices.empty();
#endif
}


bool getOutputDeviceList(const std::vector<std::string>& outputs,
                         Device::Type type, std::vector<av::Device>& devices)
{
#ifndef HAVE_FFMPEG_AVDEVICE
    SWarn << "HAVE_FFMPEG_AVDEVICE not defined, cannot list output devices";
    return false;
#else
    const AVOutputFormat* oformat = findDefaultOutputFormat(outputs);
    if (!oformat)
        return false;

    AVDeviceInfoList* devlist = nullptr;
    int error = avdevice_list_output_sinks(oformat, nullptr, nullptr, &devlist);
    if (error < 0 || !devlist) {
        LWarn("Cannot list output devices for '", oformat->name, "': ", averror(error));
        if (devlist)
            avdevice_free_list_devices(&devlist);
        return false;
    }

    devices.clear();
    for (int i = 0; i < devlist->nb_devices; i++) {
        auto dev = devlist->devices[i];
        devices.push_back(
            av::Device(type, dev->device_name, dev->device_description,
                       i == devlist->default_device));
    }

    avdevice_free_list_devices(&devlist);
    return !devices.empty();
#endif
}

#endif // HAVE_FFMPEG


void printDevices(std::ostream& ost, std::vector<Device>& devs)
{
    for (size_t i = 0; i < devs.size(); ++i) {
        ost << '\t';
        devs[i].print(ost);
        ost << '\n';
    }
}


} // namespace internal


//
// Device Manager
//


DeviceManager::DeviceManager()
    : _watcher(nullptr)
{
#ifdef HAVE_FFMPEG
    initializeFFmpeg();
#endif

    // Create platform-specific device watcher
#if defined(ICY_WIN)
    _watcher = std::make_unique<WindowsDeviceWatcher>(this);
#elif defined(ICY_APPLE)
    _watcher = std::make_unique<AppleDeviceWatcher>(this);
#elif defined(ICY_LINUX)
    _watcher = std::make_unique<LinuxDeviceWatcher>(this);
#endif
}


DeviceManager::~DeviceManager() noexcept
{
#ifdef HAVE_FFMPEG
    uninitializeFFmpeg();
#endif
}


bool DeviceManager::getCameras(std::vector<Device>& devices) const
{
    return getDeviceList(Device::VideoInput, devices);
}


bool DeviceManager::getMicrophones(std::vector<Device>& devices) const
{
    return getDeviceList(Device::AudioInput, devices);
}


bool DeviceManager::getSpeakers(std::vector<Device>& devices) const
{
    return getDeviceList(Device::AudioOutput, devices);
}


bool DeviceManager::findCamera(std::string_view name, Device& device) const
{
    std::vector<Device> devices;
    if (getCameras(devices)) {
        for (auto& dev : devices) {
            if (dev.id == name || dev.name == name) {
                device = dev;
                return true;
            }
        }
    }
    return false;
}


bool DeviceManager::findMicrophone(std::string_view name, Device& device) const
{
    std::vector<Device> devices;
    if (getMicrophones(devices)) {
        for (auto& dev : devices) {
            if (dev.id == name || dev.name == name) {
                device = dev;
                return true;
            }
        }
    }
    return false;
}


bool DeviceManager::findSpeaker(std::string_view name, Device& device) const
{
    std::vector<Device> devices;
    if (getSpeakers(devices)) {
        for (auto& dev : devices) {
            if (dev.id == name || dev.name == name) {
                device = dev;
                return true;
            }
        }
    }
    return false;
}


/// Returns the default device, preferring one marked isDefault.
/// Falls back to the first device if none is marked.
static bool getDefault(const std::vector<Device>& devices, Device& out)
{
    if (devices.empty())
        return false;

    for (auto& dev : devices) {
        if (dev.isDefault) {
            out = dev;
            return true;
        }
    }

    // No device marked as default; use first
    out = devices[0];
    return true;
}


bool DeviceManager::getDefaultCamera(Device& device) const
{
    std::vector<Device> devices;
    if (!getCameras(devices))
        return false;
    return getDefault(devices, device);
}


bool DeviceManager::getDefaultMicrophone(Device& device) const
{
    std::vector<Device> devices;
    if (!getMicrophones(devices))
        return false;
    return getDefault(devices, device);
}


bool DeviceManager::getDefaultSpeaker(Device& device) const
{
    std::vector<Device> devices;
    if (!getSpeakers(devices))
        return false;
    return getDefault(devices, device);
}


#ifdef HAVE_FFMPEG

const AVInputFormat* DeviceManager::findVideoInputFormat() const
{
    return internal::findDefaultInputFormat(ICY_VIDEO_INPUTS);
}


const AVInputFormat* DeviceManager::findAudioInputFormat() const
{
    return internal::findDefaultInputFormat(ICY_AUDIO_INPUTS);
}

#endif // HAVE_FFMPEG


bool DeviceManager::getDeviceList(Device::Type type, std::vector<av::Device>& devices) const
{
    devices.clear();

// Use native platform APIs for device enumeration where available.
// FFmpeg's avdevice implementations don't list devices properly:
// https://trac.ffmpeg.org/ticket/4486
#if defined(ICY_WIN)
    if (type == Device::VideoInput)
        return mediafoundation::getDeviceList(type, devices);
    if (type == Device::AudioInput || type == Device::AudioOutput)
        return wasapi::getDeviceList(type, devices);
#elif defined(ICY_APPLE)
    if (type == Device::VideoInput || type == Device::AudioInput)
        return avfoundation::getDeviceList(type, devices);
    if (type == Device::AudioOutput)
        return coreaudio::getDeviceList(type, devices);
#elif defined(ICY_LINUX)
    if (type == Device::VideoInput)
        return v4l2::getDeviceList(type, devices);
#endif

#ifdef HAVE_FFMPEG
    // FFmpeg avdevice fallback for output devices and Linux audio input
    switch (type) {
        case Device::VideoInput:
            return internal::getInputDeviceList(ICY_VIDEO_INPUTS, type, devices);
        case Device::AudioInput:
            return internal::getInputDeviceList(ICY_AUDIO_INPUTS, type, devices);
        case Device::VideoOutput:
            return internal::getOutputDeviceList(ICY_VIDEO_OUTPUTS, type, devices);
        case Device::AudioOutput:
            return internal::getOutputDeviceList(ICY_AUDIO_OUTPUTS, type, devices);
        default:
            LWarn("Unknown device type: ", static_cast<int>(type));
            return false;
    }
#endif

    return false;
}


int DeviceManager::getCapabilities() const
{
    std::vector<Device> devices;
    int caps = VIDEO_RECV;
    if (getMicrophones(devices))
        caps |= AUDIO_SEND;
    if (getSpeakers(devices))
        caps |= AUDIO_RECV;
    if (getCameras(devices))
        caps |= VIDEO_SEND;
    return caps;
}


void DeviceManager::setWatcher(DeviceWatcher* watcher)
{
    _watcher.reset(watcher);
}


DeviceWatcher* DeviceManager::watcher() const
{
    return _watcher.get();
}


void DeviceManager::print(std::ostream& ost) const
{
    std::vector<Device> devs;

    ost << "Video capture devices:\n";
    if (getCameras(devs)) {
        internal::printDevices(ost, devs);
    } else {
        ost << "\tNone\n";
    }

    ost << "Audio input devices:\n";
    if (getMicrophones(devs)) {
        internal::printDevices(ost, devs);
    } else {
        ost << "\tNone\n";
    }

    ost << "Audio output devices:\n";
    if (getSpeakers(devs)) {
        internal::printDevices(ost, devs);
    } else {
        ost << "\tNone\n";
    }
}


std::vector<DeviceManager::HardwareCodec> DeviceManager::getHardwareCodecs() const
{
    std::vector<HardwareCodec> result;
#ifdef HAVE_FFMPEG
    const AVCodec* codec = nullptr;
    void* opaque = nullptr;
    while ((codec = av_codec_iterate(&opaque))) {
        if (codec->capabilities & AV_CODEC_CAP_HARDWARE) {
            result.push_back({
                codec->name,
                av_codec_is_encoder(codec) ? "encoder" : "decoder"
            });
        }
    }
#endif
    return result;
}


std::optional<std::pair<Device, Device::VideoCapability>>
DeviceManager::negotiateVideoCapture(std::string_view deviceName, int width, int height, double fps) const
{
    Device device;

    if (!deviceName.empty()) {
        if (!findCamera(deviceName, device))
            return std::nullopt;
    } else {
        if (!getDefaultCamera(device))
            return std::nullopt;
    }

    return std::make_pair(device, device.bestVideoCapability(width, height, fps));
}


std::optional<std::pair<Device, Device::AudioCapability>>
DeviceManager::negotiateAudioCapture(std::string_view deviceName, int sampleRate, int channels) const
{
    Device device;

    if (!deviceName.empty()) {
        if (!findMicrophone(deviceName, device))
            return std::nullopt;
    } else {
        if (!getDefaultMicrophone(device))
            return std::nullopt;
    }

    return std::make_pair(device, device.bestAudioCapability(sampleRate, channels));
}


} // namespace av
} // namespace icy


/// @}
