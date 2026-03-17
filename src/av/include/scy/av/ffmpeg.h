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


#include "scy/base.h"
#include <memory>
#include <string>


#ifdef HAVE_FFMPEG

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


/// Initialize the FFmpeg library.
void initializeFFmpeg();

/// Uninitializes the FFmpeg library.
void uninitializeFFmpeg();

/// Get an error string for the given error code.
std::string averror(const int error);

void printInputFormats(std::ostream& ost, const char* delim = " ");
void printOutputFormats(std::ostream& ost, const char* delim = " ");
void printEncoders(std::ostream& ost, const char* delim = " ");


#ifdef HAVE_FFMPEG

//
/// Utilites for RAII:

/// Deleter adaptor for functions like av_free that take a pointer.
template <typename T, typename R, R (*Fn)(T*)>
struct Deleter
{
    inline void operator()(T* p) const
    {
        if (p)
            Fn(p);
    }
};

/// Deleter adaptor for functions like av_freep that take a pointer to a pointer.
template <typename T, typename R, R (*Fn)(T**)>
struct Deleterp
{
    inline void operator()(T* p) const
    {
        if (p)
            Fn(&p);
    }
};

using AVFrameHolder = std::unique_ptr<
    AVFrame, Deleterp<AVFrame, void, av_frame_free>>;

using AVFormatContextHolder = std::unique_ptr<
    AVFormatContext, Deleter<AVFormatContext, void, avformat_free_context>>;

using AVCodecContextHolder = std::unique_ptr<
    AVCodecContext, Deleterp<AVCodecContext, void, avcodec_free_context>>;

using AVDictionaryCleanup = std::unique_ptr<
    AVDictionary*, Deleter<AVDictionary*, void, av_dict_free>>;

using AVPacketHolder = std::unique_ptr<
    AVPacket, Deleterp<AVPacket, void, av_packet_free>>;

#endif


} // namespace av
} // namespace scy


/// @\}
