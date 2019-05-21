///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#ifndef SCY_AV_FFmpeg_H
#define SCY_AV_FFmpeg_H


#include "scy/base.h"
#include <string>
#include <memory>


#ifdef HAVE_FFMPEG

extern "C" {
#include <libavformat/avformat.h>
#ifdef HAVE_FFMPEG_AVDEVICE
#include <libavdevice/avdevice.h>
#endif
}


#define LIBAVCODEC_VERSION_CHECK( a, b, c, d, e ) \
    ( (LIBAVCODEC_VERSION_MICRO <  100 && LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( a, b, c ) ) || \
          (LIBAVCODEC_VERSION_MICRO >= 100 && LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( a, d, e ) ) )

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
template<typename T, typename R, R(*Fn)(T*)> struct Deleter {
    inline void operator() (T* p) const {
        if (p)
            Fn(p);
    }
};

/// Deleter adaptor for functions like av_freep that take a pointer to a pointer.
template<typename T, typename R, R(*Fn)(T**)> struct Deleterp {
    inline void operator() (T* p) const {
        if (p)
            Fn(&p);
    }
};

using AVFrameHolder = std::unique_ptr<
    AVFrame, Deleterp<AVFrame, void, av_frame_free>
>;

// using AVFifoBufferHolder = std::unique_ptr<
//     AVFifoBuffer, Deleter<AVFifoBuffer, void, av_fifo_free>
// >;

using AVFormatContextHolder = std::unique_ptr<
    AVFormatContext, Deleter<AVFormatContext, void, avformat_free_context>
>;

using AVCodecContextHolder = std::unique_ptr<
    AVCodecContext, Deleter<AVCodecContext, int, avcodec_close>
>;

using AVDictionaryCleanup = std::unique_ptr<
    AVDictionary*, Deleter<AVDictionary*, void, av_dict_free>
>;

// using UFileHolder = std::unique_ptr<
//     AVIOContext, Deleter<AVIOContext, int, ufile_close>
// >;

// template<typename T> using AVMallocHolder = std::unique_ptr<
//     T, Deleter<void, void, av_free>
// >;

#endif


} // namespace av
} // namespace scy


#endif // SCY_Media_FFmpeg_H


/// @\}
