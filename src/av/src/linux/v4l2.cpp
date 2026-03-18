///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/linux/v4l2.h"
#include "scy/logger.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <unistd.h>
#include <cstring>
#include <linux/videodev2.h>
#include <thread>
#include <atomic>


namespace scy {
namespace av {
namespace v4l2 {


// Map V4L2 pixel format to FFmpeg name
static const char* pixelFormatName(uint32_t fourcc)
{
    switch (fourcc) {
        case V4L2_PIX_FMT_YUYV:    return "yuyv422";
        case V4L2_PIX_FMT_UYVY:    return "uyvy422";
        case V4L2_PIX_FMT_YUV420:  return "yuv420p";
        case V4L2_PIX_FMT_NV12:    return "nv12";
        case V4L2_PIX_FMT_NV21:    return "nv21";
        case V4L2_PIX_FMT_MJPEG:   return "mjpeg";
        case V4L2_PIX_FMT_H264:    return "h264";
        case V4L2_PIX_FMT_RGB24:   return "rgb24";
        case V4L2_PIX_FMT_BGR24:   return "bgr24";
        case V4L2_PIX_FMT_RGB32:   return "rgba";
        case V4L2_PIX_FMT_BGR32:   return "bgra";
        case V4L2_PIX_FMT_GREY:    return "gray";
        default:                    return "unknown";
    }
}


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices)
{
    if (type != Device::VideoInput)
        return false;

    // Scan /dev/video* devices
    DIR* dir = opendir("/dev");
    if (!dir)
        return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strncmp(entry->d_name, "video", 5) != 0)
            continue;

        std::string path = std::string("/dev/") + entry->d_name;
        int fd = open(path.c_str(), O_RDWR | O_NONBLOCK);
        if (fd < 0)
            continue;

        // Check if it's a video capture device
        struct v4l2_capability cap = {};
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0 ||
            !(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
            close(fd);
            continue;
        }

        std::string name(reinterpret_cast<const char*>(cap.card));
        std::string busInfo(reinterpret_cast<const char*>(cap.bus_info));

        Device device(type, path, name);

        // Enumerate supported pixel formats
        struct v4l2_fmtdesc fmtdesc = {};
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
            uint32_t pixfmt = fmtdesc.pixelformat;

            // Enumerate frame sizes for this format
            struct v4l2_frmsizeenum frmsize = {};
            frmsize.pixel_format = pixfmt;

            while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0) {
                if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                    int w = static_cast<int>(frmsize.discrete.width);
                    int h = static_cast<int>(frmsize.discrete.height);

                    // Enumerate frame intervals for this size
                    struct v4l2_frmivalenum frmival = {};
                    frmival.pixel_format = pixfmt;
                    frmival.width = frmsize.discrete.width;
                    frmival.height = frmsize.discrete.height;

                    while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0) {
                        Device::VideoCapability cap;
                        cap.width = w;
                        cap.height = h;
                        cap.pixelFormat = pixelFormatName(pixfmt);

                        if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                            double fps = static_cast<double>(frmival.discrete.denominator)
                                       / frmival.discrete.numerator;
                            cap.minFps = fps;
                            cap.maxFps = fps;
                        } else if (frmival.type == V4L2_FRMIVAL_TYPE_STEPWISE ||
                                   frmival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) {
                            cap.minFps = static_cast<double>(frmival.stepwise.max.denominator)
                                       / frmival.stepwise.max.numerator;
                            cap.maxFps = static_cast<double>(frmival.stepwise.min.denominator)
                                       / frmival.stepwise.min.numerator;
                        }

                        // Avoid duplicates
                        bool dup = false;
                        for (auto& existing : device.videoCapabilities) {
                            if (existing.width == cap.width && existing.height == cap.height
                                && existing.maxFps == cap.maxFps && existing.pixelFormat == cap.pixelFormat) {
                                dup = true;
                                break;
                            }
                        }
                        if (!dup)
                            device.videoCapabilities.push_back(std::move(cap));

                        frmival.index++;
                        // Break out of stepwise/continuous after first entry
                        if (frmival.type != V4L2_FRMIVAL_TYPE_DISCRETE)
                            break;
                    }
                } else {
                    // Stepwise frame sizes: record common resolutions
                    // within the stepwise range
                    int maxW = static_cast<int>(frmsize.stepwise.max_width);
                    int maxH = static_cast<int>(frmsize.stepwise.max_height);

                    // Query frame intervals at max resolution
                    struct v4l2_frmivalenum frmival = {};
                    frmival.pixel_format = pixfmt;
                    frmival.width = static_cast<uint32_t>(maxW);
                    frmival.height = static_cast<uint32_t>(maxH);
                    double minFps = 1, maxFps = 30;

                    if (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0) {
                        if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                            maxFps = static_cast<double>(frmival.discrete.denominator)
                                   / frmival.discrete.numerator;
                            minFps = maxFps;
                        } else {
                            minFps = static_cast<double>(frmival.stepwise.max.denominator)
                                   / frmival.stepwise.max.numerator;
                            maxFps = static_cast<double>(frmival.stepwise.min.denominator)
                                   / frmival.stepwise.min.numerator;
                        }
                    }

                    Device::VideoCapability cap;
                    cap.width = maxW;
                    cap.height = maxH;
                    cap.minFps = minFps;
                    cap.maxFps = maxFps;
                    cap.pixelFormat = pixelFormatName(pixfmt);
                    device.videoCapabilities.push_back(std::move(cap));
                }

                frmsize.index++;
            }
            fmtdesc.index++;
        }

        close(fd);
        devices.push_back(std::move(device));
    }

    closedir(dir);

    // Mark first device as default
    if (!devices.empty())
        devices[0].isDefault = true;

    return !devices.empty();
}


} // namespace v4l2


//
// Linux Device Watcher (inotify-based)
//

struct LinuxDeviceWatcher::Impl
{
    std::atomic<bool> running{false};
    std::thread watchThread;
    DeviceManager* manager{nullptr};
    int pipeFd[2]{-1, -1}; // for signalling stop
};


LinuxDeviceWatcher::LinuxDeviceWatcher(DeviceManager* manager)
    : DeviceWatcher(manager)
    , _impl(std::make_unique<Impl>())
    , _manager(manager)
{
    _impl->manager = manager;
}


LinuxDeviceWatcher::~LinuxDeviceWatcher() noexcept
{
    stop();
}


bool LinuxDeviceWatcher::start()
{
    if (_impl->running)
        return true;

    if (pipe(_impl->pipeFd) < 0)
        return false;

    _impl->running = true;
    _impl->watchThread = std::thread([this]() {
        // Use inotify to watch /dev for video* device changes
        int inotifyFd = inotify_init1(IN_NONBLOCK);
        if (inotifyFd < 0) {
            _impl->running = false;
            return;
        }

        int watchFd = inotify_add_watch(inotifyFd, "/dev", IN_CREATE | IN_DELETE);
        if (watchFd < 0) {
            ::close(inotifyFd);
            _impl->running = false;
            return;
        }

        while (_impl->running) {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(inotifyFd, &fds);
            FD_SET(_impl->pipeFd[0], &fds);

            int maxFd = std::max(inotifyFd, _impl->pipeFd[0]);
            struct timeval tv = {1, 0}; // 1 second timeout

            int ret = select(maxFd + 1, &fds, nullptr, nullptr, &tv);
            if (ret <= 0)
                continue;

            if (FD_ISSET(_impl->pipeFd[0], &fds))
                break; // stop signal

            if (FD_ISSET(inotifyFd, &fds)) {
                // Read events
                char buf[4096];
                ssize_t len = read(inotifyFd, buf, sizeof(buf));
                if (len <= 0)
                    continue;

                // Check if any event is for a video device
                bool videoEvent = false;
                size_t offset = 0;
                const size_t bufLen = static_cast<size_t>(len);
                while (offset + sizeof(struct inotify_event) <= bufLen) {
                    auto* event = reinterpret_cast<struct inotify_event*>(buf + offset);
                    size_t eventSize = sizeof(struct inotify_event) + event->len;
                    if (offset + eventSize > bufLen)
                        break;
                    if (event->len > 0 && strncmp(event->name, "video", 5) == 0)
                        videoEvent = true;
                    offset += eventSize;
                }

                if (videoEvent && _impl->manager)
                    _impl->manager->DevicesChanged.emit();
            }
        }

        inotify_rm_watch(inotifyFd, watchFd);
        ::close(inotifyFd);
    });

    return true;
}


void LinuxDeviceWatcher::stop()
{
    if (!_impl->running)
        return;

    _impl->running = false;

    // Signal the watch thread to stop
    if (_impl->pipeFd[1] >= 0) {
        char c = 0;
        (void)write(_impl->pipeFd[1], &c, 1);
    }

    if (_impl->watchThread.joinable())
        _impl->watchThread.join();

    if (_impl->pipeFd[0] >= 0) { ::close(_impl->pipeFd[0]); _impl->pipeFd[0] = -1; }
    if (_impl->pipeFd[1] >= 0) { ::close(_impl->pipeFd[1]); _impl->pipeFd[1] = -1; }
}


} // namespace av
} // namespace scy


/// @\}
