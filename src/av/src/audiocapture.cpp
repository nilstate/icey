///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audiocapture.h"

#ifdef HAVE_FFMPEG

#include "icy/av/devicemanager.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}




namespace icy {
namespace av {


AudioCapture::AudioCapture()
{
}


AudioCapture::AudioCapture(const std::string& device, int channels, int sampleRate, const std::string& sampleFmt)
{
    openAudio(device, channels, sampleRate, sampleFmt);
}


AudioCapture::AudioCapture(const std::string& device, const av::AudioCodec& params)
{
    openAudio(device, params.channels, params.sampleRate);
}


AudioCapture::~AudioCapture() noexcept
{
}


void AudioCapture::openAudio(const std::string& device, const av::AudioCodec& params)
{
    openAudio(device, params.channels, params.sampleRate, params.sampleFmt);
}


void AudioCapture::openAudio(const std::string& device, int channels, int sampleRate, const std::string& sampleFmt)
{
    STrace << "Opening microphone: "
           << "device=" << device << ", "
           << "channels=" << channels << ", "
           << "sampleRate=" << sampleRate << ", "
           << "sampleFmt=" << sampleFmt;

    DeviceManager devman;
    auto iformat = devman.findAudioInputFormat();
    if (!iformat)
        throw std::runtime_error("Couldn't find microphone input format.");

    AVDictionary* iparams = nullptr;
    AVDictionaryCleanup cleanup{&iparams};

    // Set custom parameters for devices.
    // NOTE: This doesn't work for DirectShow
#ifndef ICY_WIN
    if (sampleRate > 0)
        av_dict_set_int(&iparams, "sample_rate", sampleRate, 0);
    if (channels > 0)
        av_dict_set_int(&iparams, "channels", channels, 0);

    // Set sampleFmt?
#endif

    openStream(device.c_str(), iformat, &iparams);

    // Set the decoder audio output parameters for resampler context.
    // NOTE: Must be done after `openStream`.
    // If the input device wouldn't accept our parameters then we will
    // perform pixel conversions and resizing ourself (on the decoder).
    if (_audio) {
        if (!sampleFmt.empty())
            _audio->oparams.sampleFmt = sampleFmt; // s16
        if (sampleRate > 0)
            _audio->oparams.sampleRate = sampleRate;
        if (channels > 0)
            _audio->oparams.channels = channels;
    }
}


} // namespace av
} // namespace icy


#endif


/// @\}
