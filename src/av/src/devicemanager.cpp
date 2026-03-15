///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/devicemanager.h"
#include "scy/logger.h"


#if defined(SCY_WIN)
#include "scy/av/win32/directshow.h"
#define SCY_VIDEO_INPUTS \
    {                    \
        "dshow", "vfwcap"}
#define SCY_VIDEO_OUTPUTS \
    {                     \
    }
#define SCY_SCREEN_INPUTS \
    {                     \
        "gdigrab"}
#define SCY_AUDIO_INPUTS \
    {                    \
        "dsound"}
#define SCY_AUDIO_OUTPUTS \
    {                     \
        "dsound"}
#elif defined(SCY_APPLE)
#include "scy/av/apple/avfoundation.h"
#define SCY_VIDEO_INPUTS \
    {                    \
        "avfoundation", "qtkit"}
#define SCY_VIDEO_OUTPUTS \
    {                     \
    }
#define SCY_SCREEN_INPUTS \
    {                     \
        "avfoundation"}
#define SCY_AUDIO_INPUTS \
    {                    \
        "avfoundation"}
#define SCY_AUDIO_OUTPUTS \
    {                     \
        "avfoundation"}
#elif defined(SCY_LINUX)
#define SCY_VIDEO_INPUTS \
    {                    \
        "v4l2", "dv1394"}
#define SCY_VIDEO_OUTPUTS \
    {                     \
    }
#define SCY_SCREEN_INPUTS \
    {                     \
        "x11grab"}
#define SCY_AUDIO_INPUTS \
    {                    \
        "alsa", "jack", "pulse"}
#define SCY_AUDIO_OUTPUTS \
    {                     \
        "alsa"}
#endif


using std::endl;


namespace scy {
namespace av {


//
// Device
//


Device::Device()
    : type(Unknown)
    , isDefault(false)
{
}


Device::Device(Type type, const std::string& id, const std::string& name, bool isDefault)
    : type(type)
    , id(id)
    , name(name)
    , isDefault(isDefault)
{
}


void Device::print(std::ostream& os)
{
    os << "Device[" << type << ":" << name << ":" << id << ":" << isDefault << "]";
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
        if (name == oformat->name) {
            return oformat;
        }
    }
    return nullptr;
}


const AVOutputFormat* findDefaultOutputFormat(const std::vector<std::string>& outputs)
{
    const AVOutputFormat* oformat = nullptr;
    for (auto& output : outputs) {
        oformat = findOutputFormat(output.c_str());
        if (oformat)
            break;
    }

    if (!oformat)
        throw std::runtime_error("No output format found");
    return oformat;
}


const AVInputFormat* findDefaultInputFormat(const std::vector<std::string>& inputs)
{
    const AVInputFormat* iformat = nullptr;
    for (auto& input : inputs) {
        iformat = av_find_input_format(input.c_str());
        if (iformat)
            break;
    }

    if (!iformat)
        throw std::runtime_error("No input format found");
    return iformat;
}


bool getInputDeviceList(const std::vector<std::string>& inputs,
                        Device::Type type, std::vector<av::Device>& devices)
{
#ifndef HAVE_FFMPEG_AVDEVICE
    SWarn << "HAVE_FFMPEG_AVDEVICE not defined, cannot list input devices"
          << endl;
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
            ;
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
    SWarn << "HAVE_FFMPEG_AVDEVICE not defined, cannot list output devices"
          << endl;
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
            ;
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
        ost << endl;
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
#endif // HAVE_FFMPEG
}


DeviceManager::~DeviceManager() noexcept
{
#ifdef HAVE_FFMPEG
    uninitializeFFmpeg();
#endif // HAVE_FFMPEG
}


bool DeviceManager::getCameras(std::vector<Device>& devices)
{
    return getDeviceList(Device::VideoInput, devices);
}


bool DeviceManager::getMicrophones(std::vector<Device>& devices)
{
    return getDeviceList(Device::AudioInput, devices);
}


bool DeviceManager::getSpeakers(std::vector<Device>& devices)
{
    return getDeviceList(Device::AudioOutput, devices);
}


bool DeviceManager::findCamera(const std::string& name, Device& device)
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


bool DeviceManager::findMicrophone(const std::string& name, Device& device)
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


bool DeviceManager::findSpeaker(const std::string& name, Device& device)
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


bool DeviceManager::getDefaultCamera(Device& device)
{
    std::vector<Device> devices;
    if (getCameras(devices)) {
        device = devices[0];
        return true;
    }

    return false;
}


bool DeviceManager::getDefaultMicrophone(Device& device)
{
    std::vector<Device> devices;
    if (getMicrophones(devices)) {
        device = devices[0];
        return true;
    }

    return false;
}


bool DeviceManager::getDefaultSpeaker(Device& device)
{
    std::vector<Device> devices;
    if (getSpeakers(devices)) {
        device = devices[0];
        return true;
    }

    return false;
}


#ifdef HAVE_FFMPEG

const AVInputFormat* DeviceManager::findVideoInputFormat()
{
    return internal::findDefaultInputFormat(SCY_VIDEO_INPUTS);
}


const AVInputFormat* DeviceManager::findAudioInputFormat()
{
    return internal::findDefaultInputFormat(SCY_AUDIO_INPUTS);
}

#endif // HAVE_FFMPEG


bool DeviceManager::getDeviceList(Device::Type type, std::vector<av::Device>& devices)
{
    devices.clear();

// NOTE: Unfortunately FFmpeg's dshow and avfoundation implementations don't
// list devices properly yet so we need to call native libraries outselves:
// https://trac.ffmpeg.org/ticket/4486
#if defined(SCY_WIN)
    return dshow::getDeviceList(type, devices);
#elif defined(SCY_APPLE)
    return avfoundation::getDeviceList(type, devices);
#elif defined(HAVE_FFMPEG)
    // Use FFmpeg by default
    switch (type) {
        case Device::VideoInput:
            return internal::getInputDeviceList(SCY_VIDEO_INPUTS, type, devices);
        case Device::AudioInput:
            return internal::getInputDeviceList(SCY_AUDIO_INPUTS, type, devices);
        case Device::VideoOutput:
            return internal::getOutputDeviceList(SCY_VIDEO_OUTPUTS, type, devices);
        case Device::AudioOutput:
            return internal::getOutputDeviceList(SCY_AUDIO_OUTPUTS, type, devices);
        default:
            throw std::runtime_error("Unknown device type");
    }
#endif

    return false;
}


int DeviceManager::getCapabilities()
{
    std::vector<Device> devices;
    int caps = VIDEO_RECV;
    if (getMicrophones(devices)) {
        caps |= AUDIO_SEND;
    }
    if (getSpeakers(devices)) {
        caps |= AUDIO_RECV;
    }
    if (getCameras(devices)) {
        caps |= VIDEO_SEND;
    }
    return caps;
}


void DeviceManager::setWatcher(DeviceWatcher* watcher)
{
    _watcher.reset(watcher);
}


DeviceWatcher* DeviceManager::watcher()
{
    return _watcher.get();
}


void DeviceManager::print(std::ostream& ost)
{
    std::vector<Device> devs;

    ost << "Video capture devices: " << endl;
    if (getCameras(devs)) {
        internal::printDevices(ost, devs);
    } else {
        ost << "\tNone" << endl;
    }

    ost << "Audio input devices: " << endl;
    if (getMicrophones(devs)) {
        internal::printDevices(ost, devs);
    } else {
        ost << "\tNone" << endl;
    }

    ost << "Audio output devices: " << endl;
    if (getSpeakers(devs)) {
        internal::printDevices(ost, devs);
    } else {
        ost << "\tNone" << endl;
    }
}


} // namespace av
} // namespace scy


/// @\}
