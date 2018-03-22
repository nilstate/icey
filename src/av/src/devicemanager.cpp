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


// TODO: implement all possible platform formats:
//
// input/output: alsa, decklink, fbdev, oss, pulse, sndio, v4l2
// input: avfoundation, bktr, dshow, dv1394, gdigrab, iec61883, jack, lavfi,
// openal, vfwcap, x11grab, x11grab_xcb
// output: caca, opengl, qtkit, sdl, xv

#if defined(SCY_WIN)
#include "scy/av/win32/directshow.h"
#define SCY_VIDEO_INPUTS                                                       \
    {                                                                          \
        "dshow", "vfwcap"                                                      \
    }
#define SCY_VIDEO_OUTPUTS                                                      \
    {                                                                          \
    }
#define SCY_SCREEN_INPUTS                                                      \
    {                                                                          \
        "gdigrab"                                                              \
    }
#define SCY_AUDIO_INPUTS                                                       \
    {                                                                          \
        "dsound"                                                               \
    }
#define SCY_AUDIO_OUTPUTS                                                      \
    {                                                                          \
        "dsound"                                                               \
    }
#elif defined(SCY_APPLE)
#include "scy/av/apple/avfoundation.h"
#define SCY_VIDEO_INPUTS                                                       \
    {                                                                          \
        "avfoundation", "qtkit"                                                \
    }
#define SCY_VIDEO_OUTPUTS                                                      \
    {                                                                          \
    }
#define SCY_SCREEN_INPUTS                                                      \
    {                                                                          \
        "avfoundation"                                                         \
    }
#define SCY_AUDIO_INPUTS                                                       \
    {                                                                          \
        "avfoundation"                                                         \
    }
#define SCY_AUDIO_OUTPUTS                                                      \
    {                                                                          \
        "avfoundation"                                                         \
    }
#elif defined(SCY_LINUX)
#define SCY_VIDEO_INPUTS                                                       \
    {                                                                          \
        "v4l2", "dv1394"                                                       \
    }
#define SCY_VIDEO_OUTPUTS                                                      \
    {                                                                          \
    }
#define SCY_SCREEN_INPUTS                                                      \
    {                                                                          \
        "x11grab"                                                              \
    }
#define SCY_AUDIO_INPUTS                                                       \
    {                                                                          \
        "alsa", "jack", "pulse"                                                \
    }
#define SCY_AUDIO_OUTPUTS                                                      \
    {                                                                          \
        "alsa"                                                                 \
    }
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

// Substitute for missing `av_find_output_format()` function
AVOutputFormat* findOutputFormat(const std::string& name)
{
    AVOutputFormat* oformat = av_oformat_next(nullptr);
    while (oformat) {
        if (name == oformat->name) {
            break;
        }
        oformat = av_oformat_next(oformat);
    }
    return oformat;
}


AVOutputFormat* findDefaultOutputFormat(const std::vector<std::string>& outputs)
{
    AVOutputFormat* oformat = nullptr;
    for (auto& output : outputs) {
        oformat = findOutputFormat(output.c_str());
        if (oformat)
            break;
    }

    assert(oformat && "no output format found");
    return oformat;
}


AVInputFormat* findDefaultInputFormat(const std::vector<std::string>& inputs)
{
    AVInputFormat* iformat = nullptr;
    for (auto& input : inputs) {
        iformat = av_find_input_format(input.c_str());
        if (iformat)
            break;
    }

    assert(iformat && "no input format found");
    return iformat;
}


bool enumerateDeviceList(AVFormatContext* s, Device::Type type,
                         std::vector<av::Device>& devices)
{
#ifndef HAVE_FFMPEG_AVDEVICE
    devices.clear();
    return false;
#else
    int error;
    // AVDictionary *tmp = nullptr;
    AVDeviceInfoList* devlist = nullptr;

    // av_dict_copy(&tmp, nullptr, 0);
    // if (av_opt_set_dict2(ctx, &tmp, AV_OPT_SEARCH_CHILDREN) < 0) {
    //     assert(0 && "cannot set device options");
    //     goto fail;
    // }

    // TODO: replace with `list_devices_for_context`
    error = avdevice_list_devices(s, &devlist);
    if (error || !devlist) {
        LWarn("Cannot list system devices: ", averror(error))
        goto fail;
    }

    devices.clear();
    for (int i = 0; i < devlist->nb_devices; i++) {
        auto dev = devlist->devices[i];
        devices.push_back(
            av::Device(type, dev->device_name, dev->device_description));
    }

fail:
    if (devlist)
        avdevice_free_list_devices(&devlist);

    return !devices.empty();
#endif
}


bool getInputDeviceList(const std::vector<std::string>& inputs,
                        Device::Type type, std::vector<av::Device>& devices)
{
#ifndef HAVE_FFMPEG_AVDEVICE
    SWarn << "HAVE_FFMPEG_AVDEVICE not defined, cannot list input devices"
          << endl;
    return false;
#else
    AVFormatContextHolder ctx(avformat_alloc_context());
    // AVFormatContext* ctx = nullptr;
    AVInputFormat* iformat = nullptr;

    iformat = findDefaultInputFormat(inputs);
    if (!iformat) {
        assert(0 && "no input formats");
        return false;
    }

    // Alloc an input device context
    if (!ctx) {
        assert(0);
        goto fail;
    }

    if (!iformat->priv_class ||
        !AV_IS_INPUT_DEVICE(iformat->priv_class->category)) {
        assert(0 && "not an input device");
        goto fail;
    }

    ctx->iformat = iformat;
    if (ctx->iformat->priv_data_size > 0) {
        ctx->priv_data = av_mallocz(ctx->iformat->priv_data_size);
        if (!ctx->priv_data) {
            assert(0);
            goto fail;
        }
        if (ctx->iformat->priv_class) {
            *(const AVClass**)ctx->priv_data = ctx->iformat->priv_class;
            av_opt_set_defaults(ctx->priv_data);
        }
    } else {
        ctx->priv_data = nullptr;
    }

    // // s->iformat->get_device_list
    //
    //     assert(ctx->oformat || ctx->iformat);
    //     assert((ctx->oformat && ctx->oformat->get_device_list) ||
    //         (ctx->iformat && ctx->iformat->get_device_list));

    enumerateDeviceList(ctx.get(), type, devices);

fail:
    // avformat_free_context(ctx);

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

    AVFormatContextHolder ctx(avformat_alloc_context());
    //AVFormatContext* ctx = nullptr;
    AVOutputFormat* oformat = nullptr;

    oformat = findDefaultOutputFormat(outputs);
    if (!oformat) {
        assert(0 && "no output formats");
        return false;
    }

    if (!ctx) {
        assert(0);
        goto fail;
    }

    if (!oformat->priv_class ||
        !AV_IS_OUTPUT_DEVICE(oformat->priv_class->category)) {
        assert(0 && "not an output device");
        goto fail;
    }

    ctx->oformat = oformat;
    if (ctx->oformat->priv_data_size > 0) {
        ctx->priv_data = av_mallocz(ctx->oformat->priv_data_size);
        if (!ctx->priv_data) {
            assert(0);
            goto fail;
        }
        if (ctx->oformat->priv_class) {
            *(const AVClass**)ctx->priv_data = ctx->oformat->priv_class;
            av_opt_set_defaults(ctx->priv_data);
        }
    } else {
        ctx->priv_data = nullptr;
    }

    enumerateDeviceList(ctx.get(), type, devices);

fail:
    //if (ctx)
    //    avformat_free_context(ctx);

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


DeviceManager::~DeviceManager()
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

AVInputFormat* DeviceManager::findVideoInputFormat()
{
    return internal::findDefaultInputFormat(SCY_VIDEO_INPUTS);
}


AVInputFormat* DeviceManager::findAudioInputFormat()
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
            return internal::getInputDeviceList(SCY_VIDEO_INPUTS, type,  devices);
        case Device::AudioInput:
            return internal::getInputDeviceList(SCY_AUDIO_INPUTS, type, devices);
        case Device::VideoOutput:
            return internal::getOutputDeviceList(SCY_VIDEO_OUTPUTS, type, devices);
        case Device::AudioOutput:
            return internal::getOutputDeviceList(SCY_AUDIO_OUTPUTS, type, devices);
        default:
            assert(0 && "unknown device type");
            break;
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


#if 0

// New API's below can be implemented once they have better support across more
// linux distros

bool enumerateInputDeviceList(AVInputFormat *fmt, Device::Type type, std::vector<av::Device>& devices) //AVFormatContext* s
{
    int ret, i;
    AVDeviceInfoList* devlist = nullptr;
    // AVDictionary* opts = nullptr;

    LDebug("Enumerating input devices for: ", fmt->name)
    if (!fmt || !fmt->priv_class  || !AV_IS_INPUT_DEVICE(fmt->priv_class->category)) {
        assert(0 && "not an input device");
    }

    if (!fmt->get_device_list) {
        LWarn("Cannot list input devices: Not implemented")
        goto fail;
    }

    if ((ret = avdevice_list_input_sources(fmt, nullptr, nullptr/*opts*/, &devlist)) < 0) {
        LWarn("Cannot list input device: ", averror(ret))
        goto fail;
    }

    for (int i = 0; i < devlist->nb_devices; i++) {
        auto dev = devlist->devices[i];
        // printf("%s %s [%s]\n", devlist->default_device == i ? "*" : " ",
        //        devlist->devices[i]->device_name, devlist->devices[i]->device_description);
        devices.push_back(av::Device(type,
            dev->device_name,
            dev->device_description,
            devlist->default_device == i));
    }

  fail:
    avdevice_free_list_devices(&devlist);

    return !devices.empty();
}


bool enumerateOutputDeviceList(AVOutputFormat *fmt, Device::Type type, std::vector<av::Device>& devices)
{
    int ret, i;
    AVDeviceInfoList* devlist = nullptr;
    // AVDictionary* opts = nullptr;

    LDebug("Enumerating output devices for: ", fmt->name)
    if (!fmt || !fmt->priv_class  || !AV_IS_OUTPUT_DEVICE(fmt->priv_class->category)) {
        assert(0 && "not an output device");
    }

    if (!fmt->get_device_list) {
        LWarn("Cannot list output devices: Not implemented")
        goto fail;
    }

    if ((ret = avdevice_list_output_sinks(fmt, nullptr, nullptr/*opts*/, &devlist)) < 0) {
        LWarn("Cannot list output device: ", averror(ret))
        goto fail;
    }

    for (i = 0; i < devlist->nb_devices; i++) {
        auto dev = devlist->devices[i];
        // printf("%s %s [%s]\n", devlist->default_device == i ? "*" : " ",
        //        devlist->devices[i]->device_name, devlist->devices[i]->device_description);
        devices.push_back(av::Device(type,
            dev->device_name,
            dev->device_description,
            devlist->default_device == i));
    }

  fail:
    avdevice_free_list_devices(&devlist);

    return !devices.empty();
}


AVClassCategory getAVClassCategoryFromDeviceType(Device::Type type)
{
    switch (type) {
    case Device::VideoInput: return AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT;
    case Device::AudioInput: return AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT;
    case Device::VideoOutput: return AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT;
    case Device::AudioOutput: return AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT;
    default:
        assert(0 && "unknown device type");
        break;
    }
    return AV_CLASS_CATEGORY_NA;
}


AVInputFormat* nextInputDevice(Device::Type type, AVInputFormat* prev = nullptr) //AVClassCategory c1, AVClassCategory c2
{
    const AVClass* pc;
    AVClassCategory category = AV_CLASS_CATEGORY_NA;
    AVClassCategory c1 = getAVClassCategoryFromDeviceType(type);
    AVClassCategory c2 = AV_CLASS_CATEGORY_DEVICE_INPUT;
    do {
        if (!(prev = av_iformat_next(prev)))
            break;
        pc = ((AVInputFormat *)prev)->priv_class;
        if (!pc)
            continue;
        category = pc->category;
    } while (category != c1 && category != c2);
    return prev;
}


AVOutputFormat* nextOutputDevice(Device::Type type, AVOutputFormat* prev = nullptr) //, AVClassCategory c1, AVClassCategory c2
{
    const AVClass* pc;
    AVClassCategory category = AV_CLASS_CATEGORY_NA;
    AVClassCategory c1 = getAVClassCategoryFromDeviceType(type);
    AVClassCategory c2 = AV_CLASS_CATEGORY_DEVICE_OUTPUT;
    do {
        if (!(prev = av_oformat_next(prev)))
            break;
        pc = prev->priv_class;
        if (!pc)
            continue;
        category = pc->category;
    } while (category != c1 && category != c2);
    return prev;
}


// do {
//     fmt = av_input_audio_device_next(fmt);
//     if (fmt) {
//         if (!strcmp(fmt->name, "lavfi"))
//             continue; //it's pointless to probe lavfi
//         if (dev && !av_match_name(dev, fmt->name))
//             continue;
//         print_device_sources(fmt, opts);
//     }
// } while (fmt);
// do {
//     fmt = av_input_video_device_next(fmt);
//     if (fmt) {
//         if (dev && !av_match_name(dev, fmt->name))
//             continue;
//         print_device_sources(fmt, opts);
//     }
// } while (fmt);

// do {
//     fmt = av_output_audio_device_next(fmt);
//     if (fmt) {
//         if (dev && !av_match_name(dev, fmt->name))
//             continue;
//         print_device_sinks(fmt, opts);
//     }
// } while (fmt);
// do {
//     fmt = av_output_video_device_next(fmt);
//     if (fmt) {
//         if (dev && !av_match_name(dev, fmt->name))
//             continue;
//         print_device_sinks(fmt, opts);
//     }
// } while (fmt);

#endif


} // namespace av
} // namespace scy


/// @\}
