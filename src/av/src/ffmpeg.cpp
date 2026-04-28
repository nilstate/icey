///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/ffmpeg.h"
#include "icy/logger.h"
#include "icy/util.h"

#ifdef HAVE_FFMPEG

#include <iostream>
#include <cstring>
#include <limits>
#include <mutex>
#include <stdexcept>


namespace icy {
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

        // av_register_all() and av_lockmgr_register() were removed in
        // FFmpeg 4.0+: codec/format/protocol registration is automatic.
        // libavdevice is the exception: it still requires an explicit
        // avdevice_register_all() call for av_find_input_format() to
        // resolve device backends like "avfoundation", "v4l2", "dshow".
#ifdef HAVE_FFMPEG_AVDEVICE
        avdevice_register_all();
#endif
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


AVPacketHolder makeOwnedPacket(const MediaPacket& packet,
                               int streamIndex,
                               AVRational timeBase)
{
    AVPacketHolder ffpacket(av_packet_alloc());
    if (!ffpacket)
        throw std::runtime_error("Cannot allocate FFmpeg packet");

    const auto size = packet.size();
    if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        throw std::runtime_error("FFmpeg packet too large");

    int ret = av_new_packet(ffpacket.get(), static_cast<int>(size));
    if (ret < 0)
        throw std::runtime_error("Cannot allocate FFmpeg packet data: " + averror(ret));

    if (size > 0)
        std::memcpy(ffpacket->data, packet.constData(), size);

    ffpacket->stream_index = streamIndex;
    if (packet.time > 0) {
        ffpacket->pts = av_rescale_q(packet.time, AVRational{1, AV_TIME_BASE},
                                     timeBase);
        ffpacket->dts = ffpacket->pts;
    }
    else {
        ffpacket->pts = AV_NOPTS_VALUE;
        ffpacket->dts = AV_NOPTS_VALUE;
    }

    return ffpacket;
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
} // namespace icy


#else

namespace icy {
namespace av {


void initializeFFmpeg()
{
}


void uninitializeFFmpeg()
{
}


} // namespace av
} // namespace icy


#endif


/// @}
