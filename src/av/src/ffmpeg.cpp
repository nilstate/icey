///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/ffmpeg.h"
#include "scy/logger.h"
#include "scy/util.h"

#ifdef HAVE_FFMPEG

#include <iostream>
#include <mutex>
#include <stdexcept>


namespace scy {
namespace av {
namespace internal {


static std::mutex _mutex;
static int _refCount(0);


[[maybe_unused]] static void logCallback(void* ptr, int level, const char* fmt, va_list vl)
{
    char logbuf[256];
    vsnprintf(logbuf, sizeof(logbuf), fmt, vl);
    logbuf[sizeof(logbuf) - 1] = '\0';

    switch (level) {
        case AV_LOG_PANIC:
        case AV_LOG_FATAL:
        case AV_LOG_ERROR:
            LError("FFmpeg: ", util::trimRight<std::string>(logbuf));
            break;
        case AV_LOG_WARNING:
            LWarn("FFmpeg: ", util::trimRight<std::string>(logbuf));
            break;
        case AV_LOG_INFO:
            LDebug("FFmpeg: ", util::trimRight<std::string>(logbuf));
            break;
        default:
        case AV_LOG_VERBOSE:
        case AV_LOG_DEBUG:
            // LTrace("FFmpeg: ", util::trimRight<std::string>(logbuf));
            break;
    }
}


void init()
{
    std::lock_guard<std::mutex> guard(_mutex);

    if (++_refCount == 1) {
        // Use an internal log callback.
        // av_log_set_callback(logCallback);
        // av_log_set_level(AV_LOG_INFO);

        // Optionally disable logging.
        // av_log_set_level(AV_LOG_QUIET);

        // Note: av_register_all(), av_lockmgr_register(), and
        // avdevice_register_all() were removed in FFmpeg 4.0+/5.0+.
        // Codec, format, and device registration is now automatic.
    }
}


void uninit()
{
    std::lock_guard<std::mutex> guard(_mutex);
    --_refCount;
}


} // namespace internal


void initializeFFmpeg()
{
    internal::init();
}


void uninitializeFFmpeg()
{
    internal::uninit();
}


std::string averror(const int error)
{
    char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
}


void printInputFormats(std::ostream& ost, const char* delim)
{
    const AVInputFormat* p = nullptr;
    void* opaque = nullptr;
    while ((p = av_demuxer_iterate(&opaque))) {
        ost << p->name << delim;
    }
}


void printOutputFormats(std::ostream& ost, const char* delim)
{
    const AVOutputFormat* p = nullptr;
    void* opaque = nullptr;
    while ((p = av_muxer_iterate(&opaque))) {
        ost << p->name << delim;
    }
}


void printEncoders(std::ostream& ost, const char* delim)
{
    const AVCodec* p = nullptr;
    void* opaque = nullptr;
    while ((p = av_codec_iterate(&opaque))) {
        if (av_codec_is_encoder(p))
            ost << p->name << delim;
    }
}


} // namespace av
} // namespace scy


#else

namespace scy {
namespace av {


void initializeFFmpeg()
{
}


void uninitializeFFmpeg()
{
}


} // namespace av
} // namespace scy


#endif


/// @\}
