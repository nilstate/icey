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


#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
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

    /// A video format supported by the device.
    struct VideoCapability
    {
        int width{0};
        int height{0};
        double minFps{0};
        double maxFps{0};
        std::string pixelFormat; // "yuv420p", "nv12", "mjpeg", etc.
    };

    /// An audio format supported by the device.
    struct AudioCapability
    {
        int sampleRate{0};
        int channels{0};
        std::string sampleFormat; // "s16", "f32", "fltp", etc.
    };

    Type type{Unknown};
    std::string id;
    std::string name;
    bool isDefault{false};
    bool isConnected{true};
    bool isInUse{false};

    std::vector<VideoCapability> videoCapabilities;
    std::vector<AudioCapability> audioCapabilities;

    Device();
    Device(Type type, const std::string& id, const std::string& name, bool isDefault = false);

    void print(std::ostream& os) const;

    bool operator==(const Device& that) const
    {
        return type == that.type && id == that.id && name == that.name;
    }

    /// Find the video capability closest to the requested parameters.
    ///
    /// Scoring: resolution match weighted 70%, fps match weighted 30%.
    /// Both dimensions are normalised to [0,1] so the weights are meaningful.
    /// Prefers capabilities that are >= the requested resolution (upscaling
    /// is worse than slight downscaling).
    ///
    /// Returns a default capability with the requested values if none are
    /// available (allows callers to skip the empty check).
    VideoCapability bestVideoCapability(int width, int height, double fps) const
    {
        if (videoCapabilities.empty())
            return VideoCapability{width, height, fps, fps, "yuv420p"};

        const double targetArea = static_cast<double>(width * height);
        const double targetFps = std::max(1.0, fps);

        const VideoCapability* best = &videoCapabilities[0];
        double bestScore = std::numeric_limits<double>::max();

        for (auto& cap : videoCapabilities) {
            double capArea = static_cast<double>(cap.width * cap.height);

            // Normalised resolution distance [0, inf)
            // Penalise undersized resolutions more (1.5x weight)
            double resDist = std::abs(capArea - targetArea) / targetArea;
            if (capArea < targetArea)
                resDist *= 1.5;

            // Normalised fps distance [0, inf)
            double fpsDist = std::abs(cap.maxFps - fps) / targetFps;

            double score = resDist * 0.7 + fpsDist * 0.3;
            if (score < bestScore) {
                bestScore = score;
                best = &cap;
            }
        }
        return *best;
    }

    /// Find the audio capability closest to the requested parameters.
    ///
    /// Scoring: sample rate match weighted 70%, channel count weighted 30%.
    /// Both dimensions are normalised to [0,1].
    ///
    /// Returns a default capability with the requested values if none are
    /// available.
    AudioCapability bestAudioCapability(int sampleRate, int channels) const
    {
        if (audioCapabilities.empty())
            return AudioCapability{sampleRate, channels, "s16"};

        const double targetRate = static_cast<double>(std::max(1, sampleRate));
        const double targetCh = static_cast<double>(std::max(1, channels));

        const AudioCapability* best = &audioCapabilities[0];
        double bestScore = std::numeric_limits<double>::max();

        for (auto& cap : audioCapabilities) {
            double rateDist = std::abs(cap.sampleRate - sampleRate) / targetRate;
            double chDist = std::abs(cap.channels - channels) / targetCh;
            double score = rateDist * 0.7 + chDist * 0.3;
            if (score < bestScore) {
                bestScore = score;
                best = &cap;
            }
        }
        return *best;
    }
};


/// Bitmask of media capabilities detected on this system.
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

/// Base class for platform-specific device change monitors.
///
/// Call start() to begin monitoring and stop() to end.
/// The watcher is NOT auto-started; callers must explicitly start it.
class AV_API DeviceWatcher
{
public:
    explicit DeviceWatcher(DeviceManager*) {}
    virtual ~DeviceWatcher() noexcept = default;
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
    ~DeviceManager() noexcept;

    /// Device enumeration
    bool getCameras(std::vector<Device>& devices) const;
    bool getMicrophones(std::vector<Device>& devices) const;
    bool getSpeakers(std::vector<Device>& devices) const;

    /// Default devices (returns first device, with isDefault preferred)
    bool getDefaultCamera(Device& device) const;
    bool getDefaultMicrophone(Device& device) const;
    bool getDefaultSpeaker(Device& device) const;

    /// Find device by name or id
    bool findCamera(std::string_view name, Device& device) const;
    bool findMicrophone(std::string_view name, Device& device) const;
    bool findSpeaker(std::string_view name, Device& device) const;

    /// Base device list populated by platform-specific backends.
    bool getDeviceList(Device::Type type, std::vector<av::Device>& devices) const;

    /// Bitmask of detected media capabilities.
    int getCapabilities() const;

    void setWatcher(DeviceWatcher* watcher);
    DeviceWatcher* watcher() const;

    /// Print all devices to the output stream.
    void print(std::ostream& ost) const;

    /// Returns the default `DeviceManager` singleton.
    static DeviceManager& instance();

    /// Shuts down the `MediaFactory` and deletes the singleton instance.
    static void shutdown();

    /// Signals when a system device is connected or removed.
    /// Emitted by the platform-specific `DeviceWatcher`.
    ///
    /// NOTE: This signal may be emitted from a background thread
    /// (e.g. inotify thread on Linux, COM callback on Windows,
    /// CoreAudio listener on macOS). Handlers must be thread-safe
    /// and should NOT call back into DeviceManager to enumerate.
    /// Instead, post to the main thread or set a flag.
    NullSignal DevicesChanged;

    /// Hardware codec descriptor.
    struct HardwareCodec
    {
        std::string name; // FFmpeg codec name (e.g. "h264_nvenc")
        std::string type; // "encoder" or "decoder"
    };

    /// Detect available hardware-accelerated codecs via FFmpeg.
    std::vector<HardwareCodec> getHardwareCodecs() const;

    /// Resolve the best video capture parameters for a device.
    ///
    /// Looks up the device by name/id (or default camera if empty),
    /// queries its capabilities, and returns the closest match.
    /// Returns std::nullopt if no camera is available.
    ///
    /// Usage:
    ///   if (auto result = devman.negotiateVideoCapture("", 1280, 720, 30)) {
    ///       auto& [dev, cap] = *result;
    ///       capture.openVideo(dev.id, cap.width, cap.height, cap.maxFps, cap.pixelFormat);
    ///   }
    std::optional<std::pair<Device, Device::VideoCapability>>
    negotiateVideoCapture(std::string_view deviceName, int width, int height, double fps) const;

    /// Resolve the best audio capture parameters for a device.
    /// Returns std::nullopt if no microphone is available.
    std::optional<std::pair<Device, Device::AudioCapability>>
    negotiateAudioCapture(std::string_view deviceName, int sampleRate, int channels) const;

#ifdef HAVE_FFMPEG
    /// Find base FFmpeg formats
    const AVInputFormat* findVideoInputFormat() const;
    const AVInputFormat* findAudioInputFormat() const;
#endif

protected:
    std::unique_ptr<DeviceWatcher> _watcher;
};


} // namespace av
} // namespace scy
