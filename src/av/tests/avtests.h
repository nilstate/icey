///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//


#ifndef SCY_AV_Tests_H
#define SCY_AV_Tests_H


#include "scy/av/audiobuffer.h"
#include "scy/av/audiocapture.h"
#include "scy/av/audiodecoder.h"
#include "scy/av/audioencoder.h"
#include "scy/av/audioresampler.h"
#include "scy/av/devicemanager.h"
#include "scy/av/format.h"
#include "scy/av/formatregistry.h"
#include "scy/av/fpscounter.h"
#include "scy/av/mediacapture.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/av/realtimepacketqueue.h"
#include "scy/av/videocapture.h"
#include "scy/av/videodecoder.h"
#include "scy/av/videoencoder.h"
#include "scy/base.h"
#include "scy/filesystem.h"
#include "scy/logger.h"
#include "scy/test.h"
#include "scy/util.h"
#include <chrono>
#include <random>

#ifdef HAVE_FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#endif


using scy::test::Test;
using std::cerr;
using std::cout;
using std::endl;


namespace scy {


static const int kNumberFramesWanted = 200;
static const int kInNumSamples = 1024;

#ifdef SCY_WIN
// Use native aac encoder on Windows since newer FFmpeg
// version should always be available.
#define ACC_ENCODER "aac"
#else
// Use libfdk_aac on Linux since native FFmpeg aac
// encoder is shaky on older versions.
#define ACC_ENCODER "libfdk_aac"
#endif

#define MP4_H264_AAC_TRANSCODER_FORMAT av::Format{"MP4 Default", "mp4", {"libx264", 400, 300}, {ACC_ENCODER}};

#define MP4_H264_AAC_REALTIME_FORMAT av::Format{"MP4 Realtime", "mp4", {"libx264", 400, 300, 25, 48000, 128000, "yuv420p"}, {ACC_ENCODER, 2, 44100, 64000, "fltp"}};

#define VP8_H264_AAC_REALTIME_FORMAT av::Format{"MP4 VP8 Realtime", "mp4", {"vp8", 400, 300, 25, 48000, 128000, "yuv420p"}, {ACC_ENCODER, 2, 44100, 64000, "fltp"}};


// =============================================================================
// Helpers
//

std::string sampleDataDir(const std::string& file)
{
    std::string dir(SCY_DATA_DIR);
    if (!file.empty())
        fs::addnode(dir, file);
    return dir;
}

// Prepare a dummy YUV image
#ifdef HAVE_FFMPEG
// static void fillYuvImage(AVFrame* pict, int frame_index, int width, int height)
// {
//     int x, y, i;
//     i = frame_index;
//
//     /* Y */
//     for (y = 0; y < height; y++) {
//         for (x = 0; x < width; x++) {
//             pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
//         }
//     }
//
//     /* Cb and Cr */
//     for (y = 0; y < height / 2; y++) {
//         for (x = 0; x < width / 2; x++) {
//             pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
//             pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
//         }
//     }
// }

// Generate sin tone with 440Hz frequency and duplicated channels
void fillAudioSamplesUInt(uint16_t* samples, int sample_rate,
                          int nb_channels, int nb_samples, double* t)
{
    int j, k;
    float tincr = 2 * M_PI * 440.0 / sample_rate;
    for (j = 0; j < nb_samples; j++) {
        samples[2 * j] = static_cast<int>(sin(*t) * 10000);
        for (k = 1; k < nb_channels; k++)
            samples[2 * j + k] = samples[2 * j];
        *t += tincr;
    }
}

// Generate sin tone with 440Hz frequency and duplicated channels
void fillAudioSamples(double* samples, int sample_rate, int nb_channels,
                      int nb_samples, double* t)
{
    int i, j;
    double tincr = 1.0 / sample_rate, *dstp = samples;
    const double c = 2 * M_PI * 440.0;
    for (i = 0; i < nb_samples; i++) {
        *dstp = sin(c * *t);
        for (j = 1; j < nb_channels; j++)
            dstp[j] = dstp[0];
        dstp += nb_channels;
        *t += tincr;
    }
}


std::vector<uint16_t*> createTestAudioSamplesS16(int numFrames, int nbSamples, const av::AudioCodec& params)
{
    double t = 0;
    int bufferSize = av_samples_get_buffer_size(
        nullptr, params.channels, nbSamples,
        av_get_sample_fmt(params.sampleFmt.c_str()), 0);
    auto vec = std::vector<uint16_t*>();
    do {
        auto samples = new uint16_t[bufferSize];
        fillAudioSamplesUInt(samples, params.sampleRate, params.channels, nbSamples, &t);
        vec.push_back(samples);
    } while (--numFrames);
    return vec;
}


std::vector<double*> createTestAudioSamplesDBL(int numFrames, int nbSamples, const av::AudioCodec& params)
{
    double t = 0;
    int bufferSize = av_samples_get_buffer_size(
        nullptr, params.channels, nbSamples,
        av_get_sample_fmt(params.sampleFmt.c_str()), 0);
    auto vec = std::vector<double*>();
    do {
        auto samples = new double[bufferSize];
        fillAudioSamples(samples, params.sampleRate, params.channels, nbSamples, &t);
        vec.push_back(samples);
    } while (--numFrames);
    return vec;
}
#endif


#ifdef HAVE_FFMPEG


// =============================================================================
// Video File Transcoder
//

class VideoFileTranscoderTest : public Test
{
    void run()
    {
        av::EncoderOptions options;
        options.ofile = "transcoderoutput.mp4";
        options.oformat = MP4_H264_AAC_REALTIME_FORMAT;

        // Create a media capture to read and decode the input file
        auto capture(std::make_shared<av::MediaCapture>());
        capture->openFile(sampleDataDir("test.mp4"));
        capture->getEncoderFormat(options.iformat);
        // options.iformat.audio.enabled = false;
        // options.iformat.video.enabled = false;

        // Create the multiplex encoder
        auto encoder(std::make_shared<av::MultiplexPacketEncoder>(options));

        // Create a PacketStream to pass packets
        // from the file capture to the encoder
        PacketStream stream;
        stream.attachSource(capture, true);
        stream.attach(encoder);
        stream.start();

        while (!capture->stopping()) {
            LDebug("Waiting for completion");
            scy::sleep(10);
        }

        // Check output file size
        expect(fs::filesize(options.ofile) > 10000);

        // NOTE: Disabling time checks for now since different
        // FFmpeg versions produce different results,
        //
        // // capture video time: 59958333
        // // capture video ctx->frame_number: 1440
        // // capture audio time: 60093242
        // // capture audio ctx->frame_number: 1295
        // int captureAudioSeconds = 60;
        // int captureVideoSeconds = 59;
        // expect(capture->audio()->time > captureAudioSeconds * time::kNumMicrosecsPerSec);
        // expect(capture->video()->time > captureVideoSeconds * time::kNumMicrosecsPerSec);
        // expect(capture->audio()->time < (captureAudioSeconds + 1) * time::kNumMicrosecsPerSec);
        // expect(capture->video()->time < (captureVideoSeconds + 1) * time::kNumMicrosecsPerSec);
        // expect(capture->audio()->seconds > captureAudioSeconds);
        // expect(capture->video()->seconds > captureVideoSeconds);
        // expect(capture->audio()->seconds < (captureAudioSeconds + 1));
        // expect(capture->video()->seconds < (captureVideoSeconds + 1));
        //
        // // encoder video: 57416640
        // // encoder video ctx->frame_number: 1440
        // // encoder audio time: 60046802
        // // encoder audio ctx->frame_number: 2589
        // int encoderAudioSeconds = 60;
        // int encoderVideoSeconds = 57;
        // expect(encoder->audio()->time > encoderAudioSeconds * time::kNumMicrosecsPerSec);
        // expect(encoder->video()->time > encoderVideoSeconds * time::kNumMicrosecsPerSec);
        // expect(encoder->audio()->time < (encoderAudioSeconds + 1) * time::kNumMicrosecsPerSec);
        // expect(encoder->video()->time < (encoderVideoSeconds + 1) * time::kNumMicrosecsPerSec);
        // expect(encoder->audio()->seconds > encoderAudioSeconds);
        // expect(encoder->video()->seconds > encoderVideoSeconds);
        // expect(encoder->audio()->seconds < (encoderAudioSeconds + 1));
        // expect(encoder->video()->seconds < (encoderVideoSeconds + 1));
    }
};

// =============================================================================
// Video Multiplex Capture Encoder
//

class MultiplexCaptureEncoderTest : public Test
{
    void run()
    {
        av::EncoderOptions options;
        options.ofile = "multiplexcaptureencoderoutput.mp4";
        options.oformat = MP4_H264_AAC_REALTIME_FORMAT;
        options.iformat.audio.enabled = false; // enabled if available
        options.iformat.video.enabled = false; // enabled if available

        // Create a PacketStream to pass packets from the
        // file capture to the encoder
        PacketStream stream;

        // Create a device manager instance to enumerate system devices
        av::Device device;
        av::DeviceManager devman;

        // Create and attach the default video capture
        av::VideoCapture video;
        if (devman.getDefaultCamera(device)) {
            LInfo("Using video device: ", device.name);
            video.openVideo(device.id, 640, 480);
            video.getEncoderFormat(options.iformat);
            stream.attachSource(&video, false, true);
        }

        // Create and attach the default audio capture
        av::AudioCapture audio;
        if (devman.getDefaultMicrophone(device)) {
            LInfo("Using audio device: ", device.name);
            audio.openAudio(device.id, 2, 44100);
            audio.getEncoderFormat(options.iformat);
            stream.attachSource(&audio, false, true);
        }

        // Skip test if no devices are available
        if (!options.iformat.audio.enabled && !options.iformat.video.enabled) {
            std::cout << "no camera or microphone available, skipping..." << endl;
            return;
        }

        // Create the multiplex encoder
        auto encoder(std::make_shared<av::MultiplexPacketEncoder>(options));
        stream.attach(encoder, 5);
        stream.start();

        // Encode 100 frames and break
        while (video.video()->seconds < 10 && !video.stopping() && !audio.stopping()) {
            // LDebug("Waiting for completion: ", numFramesRemaining);
            scy::sleep(10);
        }

        // Stop and flush the stream
        stream.stop();

        // Check output file size
        expect(fs::filesize(options.ofile) > 10000);

        // Verify output file data integrity
        {
            AVFormatContext* fmtCtx = nullptr;
            int ret = avformat_open_input(&fmtCtx, options.ofile.c_str(), nullptr, nullptr);
            expect(ret == 0);
            if (ret == 0) {
                ret = avformat_find_stream_info(fmtCtx, nullptr);
                expect(ret >= 0);
                expect(fmtCtx->nb_streams > 0);

                // Verify video stream exists if video was enabled
                if (options.iformat.video.enabled) {
                    int vidIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
                    expect(vidIdx >= 0);
                }

                // Verify audio stream exists if audio was enabled
                if (options.iformat.audio.enabled) {
                    int audIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
                    expect(audIdx >= 0);
                }

                // Verify file has non-zero duration
                expect(fmtCtx->duration > 0);

                avformat_close_input(&fmtCtx);
            }
        }
    }
};

// =============================================================================
// Audio Encoder
//
class AudioEncoderTest : public Test
{
    int numFramesEncoded = 0;
    std::ofstream output;

    void run()
    {
        av::AudioEncoder encoder;
        auto& iparams = encoder.iparams;
        auto& oparams = encoder.oparams;

        iparams.channels = 2;
        iparams.sampleRate = 48000;
        iparams.sampleFmt = "dbl";

        oparams.encoder = "mp2"; // mp2, aac, libfdk_aac
        oparams.bitRate = 64000;
        oparams.channels = 2;
        oparams.sampleRate = 44100;
        oparams.sampleFmt = "s16"; // fltp
        oparams.enabled = true;

        encoder.emitter += packetSlot(this, &AudioEncoderTest::onAudioEncoded);
        encoder.create();
        encoder.open();

        output.open("test." + oparams.encoder, std::ios::out | std::ios::binary);

        auto testSamples = createTestAudioSamplesDBL(kNumberFramesWanted, kInNumSamples, iparams);
        for (auto samples : testSamples) {
            encoder.encode(reinterpret_cast<uint8_t*>(samples), kInNumSamples, AV_NOPTS_VALUE);
        }

        encoder.flush();
        encoder.close();
        output.close();
        util::clearVector<>(testSamples);

        // Verify encoded audio output integrity
        expect(numFramesEncoded > 0);
        {
            std::string outputFile = "test." + oparams.encoder;
            AVFormatContext* fmtCtx = nullptr;
            int ret = avformat_open_input(&fmtCtx, outputFile.c_str(), nullptr, nullptr);
            expect(ret == 0);
            if (ret == 0) {
                ret = avformat_find_stream_info(fmtCtx, nullptr);
                expect(ret >= 0);

                int audIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
                expect(audIdx >= 0);
                if (audIdx >= 0) {
                    auto* codecpar = fmtCtx->streams[audIdx]->codecpar;
                    expect(codecpar->ch_layout.nb_channels == oparams.channels);
                    expect(codecpar->sample_rate == oparams.sampleRate);
                }

                avformat_close_input(&fmtCtx);
            }
        }
    }

    void onAudioEncoded(av::AudioPacket& packet)
    {
        numFramesEncoded++;
        output.write(reinterpret_cast<const char*>(packet.data()), packet.size());
    }
};


// =============================================================================
// Audio Resampler
//
class AudioResamplerTest : public Test
{
    void run()
    {
        av::AudioResampler resampler;
        auto& iparams = resampler.iparams;
        auto& oparams = resampler.oparams;

        iparams.channels = 2;
        iparams.sampleRate = 48000;
        iparams.sampleFmt = "dbl";

        oparams.channels = 2;
        oparams.sampleRate = 44100;
        oparams.sampleFmt = "s16";

        resampler.open();

        std::ofstream output("test.pcm", std::ios::out | std::ios::binary);

        auto testSamples = createTestAudioSamplesDBL(kNumberFramesWanted * 100, kInNumSamples, iparams);
        for (auto samples : testSamples) {
            // auto data = reinterpret_cast<uint8_t*>(samples);
            if (resampler.resample(reinterpret_cast<uint8_t**>(&samples), kInNumSamples)) {
                output.write(
                    reinterpret_cast<const char*>(resampler.outSamples[0]),
                    resampler.outBufferSize);
            }
        }

        resampler.close();
        output.close();
        util::clearVector<>(testSamples);

        // Verify resampled PCM output has expected size
        // Output is s16 (2 bytes per sample), 2 channels, resampled from 48000 to 44100 Hz
        auto fileSize = fs::filesize("test.pcm");
        expect(fileSize > 0);

        // Verify file size is in the expected ballpark:
        // input frames * samples_per_frame * (44100/48000) ratio * channels * bytes_per_sample
        int64_t expectedMinSize = static_cast<int64_t>(kNumberFramesWanted * 100) * kInNumSamples
            * oparams.channels * 2 /* s16 = 2 bytes */
            * 44100 / 48000
            * 80 / 100; // allow 20% tolerance
        expect(fileSize > expectedMinSize);
    }
};


// =============================================================================
// Audio Fifo Buffer
//
class AudioBufferTest : public Test
{
    void run()
    {
        av::AudioBuffer fifo;
        fifo.alloc("dbl", 2);

        double t = 0;
        auto samples = new double[4096];
        fillAudioSamples(samples, 48000, 2, 200, &t);

        fifo.write((void**)&samples, 200);
        expect(fifo.available() == 200);

        fifo.read((void**)&samples, 100);
        expect(fifo.available() == 100);

        fifo.read((void**)&samples, 100);
        expect(fifo.available() == 0);

        delete samples;
    }
};


// =============================================================================
// Audio Capture
//
class AudioCaptureTest : public Test
{
    int inNbChannels = 2;
    int inSampleRate = 48000;
    int numFramesRemaining = kNumberFramesWanted;
    std::ofstream output;

    void run()
    {
        output.open("test.pcm", std::ios::out | std::ios::binary);

        av::Device device;
        av::DeviceManager devman;
        if (!devman.getDefaultMicrophone(device)) {
            std::cout << "no microphone available, skipping..." << endl;
            return;
        }

        // Create a media capture to read and decode the input file
        // av::MediaCapture capture;
        // capture.openFile("test.mp4");
        av::AudioCapture capture(device.id, inNbChannels, inSampleRate);

        capture.emitter.attach(packetSlot(this, &AudioCaptureTest::onAudioCaptured));
        capture.start();

        av::AudioCodec oparams;
        capture.getEncoderAudioCodec(oparams);
        expect(oparams.channels == inNbChannels);
        expect(oparams.sampleRate == inSampleRate);

        while (numFramesRemaining > 0 && !capture.stopping()) {
            LDebug("Waiting for completion: ", numFramesRemaining);
            scy::sleep(10);
        }

        output.close();
    }

    void onAudioCaptured(av::AudioPacket& packet)
    {
        STrace << "On audio packet: samples=" << packet.numSamples
               << ", time=" << packet.time << endl;
        if (numFramesRemaining) {
            numFramesRemaining--;
            output.write(reinterpret_cast<const char*>(packet.data()), packet.size());
        }
    }
};


// =============================================================================
// Audio Capture Encoder
//
class AudioCaptureEncoderTest : public Test
{
    int inNbChannels = 2;
    int inSampleRate = 48000; // 22050, 48000
    int numFramesRemaining = kNumberFramesWanted;
    av::AudioEncoder encoder;
    std::ofstream output;

    void run()
    {
        auto& iparams = encoder.iparams;
        auto& oparams = encoder.oparams;

        av::Device device;
        av::DeviceManager devman;
        if (!devman.getDefaultMicrophone(device)) {
            std::cout << "no microphone available, skipping..." << endl;
            return;
        }

        // Create a media capture to read and decode the input file
        av::AudioCapture capture(device.id, inNbChannels, inSampleRate);

        capture.getEncoderAudioCodec(iparams);
        capture.start();

        oparams.encoder = "mp2"; // mp2, aac, libfdk_aac
        oparams.bitRate = 64000;
        oparams.channels = 2;
        oparams.sampleRate = 44100; // 22050
        oparams.sampleFmt = "s16";  // fltp
        oparams.enabled = true;

        output.open("test." + oparams.encoder, std::ios::out | std::ios::binary);

        encoder.create();
        encoder.open();

        capture.emitter.attach(packetSlot(this, &AudioCaptureEncoderTest::onAudioCaptured));
        encoder.emitter.attach(packetSlot(this, &AudioCaptureEncoderTest::onAudioEncoded));

        expect(iparams.channels == inNbChannels);
        expect(iparams.sampleRate == inSampleRate);

        while (numFramesRemaining > 0 && !capture.stopping()) {
            LDebug("Waiting for completion: ", numFramesRemaining);
            scy::sleep(10);
        }

        LDebug("Number samples remaining: ", encoder.fifo.available());
        // assert(encoder.fifo.available() == 0);
        expect(encoder.fifo.available() < 1024);

        capture.stop();
        encoder.close();
        output.close();

        // Verify encoded audio output integrity
        expect(numFramesRemaining == 0);
        {
            std::string outputFile = "test." + oparams.encoder;
            AVFormatContext* fmtCtx = nullptr;
            int ret = avformat_open_input(&fmtCtx, outputFile.c_str(), nullptr, nullptr);
            expect(ret == 0);
            if (ret == 0) {
                ret = avformat_find_stream_info(fmtCtx, nullptr);
                expect(ret >= 0);

                int audIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
                expect(audIdx >= 0);
                if (audIdx >= 0) {
                    auto* codecpar = fmtCtx->streams[audIdx]->codecpar;
                    expect(codecpar->ch_layout.nb_channels == oparams.channels);
                    expect(codecpar->sample_rate == oparams.sampleRate);
                }

                avformat_close_input(&fmtCtx);
            }
        }
    }

    void onAudioCaptured(av::AudioPacket& packet)
    {
        STrace << "On audio packet: samples=" << packet.numSamples
               << ", time=" << packet.time << endl;
        encoder.encode(reinterpret_cast<uint8_t*>(packet.data()), packet.numSamples, AV_NOPTS_VALUE);
    }

    void onAudioEncoded(av::AudioPacket& packet)
    {
        if (numFramesRemaining) {
            numFramesRemaining--;
            output.write(reinterpret_cast<const char*>(packet.data()), packet.size());
        }
    }
};

// =============================================================================
// Audio Capture Resampler
//
// The output file is playable with the following command:
// ffplay -f s16le -channel_layout 3 -channels 2 -ar 44100 test.pcm
//
class AudioCaptureResamplerTest : public Test
{
    int inNbChannels = 2;
    int inSampleRate = 48000;
    int numFramesRemaining = kNumberFramesWanted; // * 100;
    av::AudioResampler resampler;
    std::ofstream output;

    void run()
    {
        auto& iparams = resampler.iparams;
        auto& oparams = resampler.oparams;

        oparams.channels = 2;
        oparams.sampleRate = 44100;
        oparams.sampleFmt = "s16";

        av::Device device;
        av::DeviceManager devman;
        if (!devman.getDefaultMicrophone(device)) {
            std::cout << "no microphone available, skipping..." << endl;
            return;
        }

        av::AudioCapture capture(device.id, inNbChannels, inSampleRate);

        capture.getEncoderAudioCodec(iparams);
        capture.start();

        expect(iparams.channels == inNbChannels);
        expect(iparams.sampleRate == inSampleRate);

        output.open("test.pcm", std::ios::out | std::ios::binary);

        resampler.open();
        capture.emitter.attach(packetSlot(this, &AudioCaptureResamplerTest::onAudioCaptured));

        while (numFramesRemaining > 0) {
            LDebug("Waiting for completion: ", numFramesRemaining);
            scy::sleep(10);
        }

        capture.stop();
        resampler.close();
        output.close();
        LDebug("numFramesRemaining: ", numFramesRemaining);

        // Verify resampled PCM output has non-zero size
        expect(numFramesRemaining == 0);
        expect(fs::filesize("test.pcm") > 0);
    }

    void onAudioCaptured(av::AudioPacket& packet)
    {
        if (numFramesRemaining) {
            numFramesRemaining--;
            LDebug("On audio packet: ", packet.size());
            auto data = reinterpret_cast<uint8_t*>(packet.data());
            if (resampler.resample(&data, packet.numSamples)) {
                output.write(
                    reinterpret_cast<const char*>(resampler.outSamples[0]),
                    resampler.outBufferSize);
            }
        }
    }
};


// class RealtimeMediaQueueEncoderTest : public Test
// {
//     PacketStream stream;
//     int numFramesRemaining = 1000; // kNumberFramesWanted;
//
//     void run()
//     {
//         auto capture = std::make_shared<av::MediaCapture>();
//         capture->openFile("test.mp4");
//         stream.attachSource(capture, true);
//         stream.attach(std::make_shared<av::RealtimePacketQueue<av::MediaPacket>>());
//         stream.start();
//         stream.emitter += packetSlot(this, &RealtimeMediaQueueEncoderTest::onPacketPlayout);
//
//         while (numFramesRemaining > 0) {
//             LDebug("Waiting for completion: ", numFramesRemaining);
//             scy::sleep(10);
//         }
//
//         // Verify stream processed all expected frames
//         expect(numFramesRemaining == 0);
//     }
//
//     void onPacketPlayout(av::MediaPacket& packet)
//     {
//         if (numFramesRemaining) {
//             numFramesRemaining--;
//             LDebug("On packet: ", numFramesRemaining, ": ", packet.time);
//             // LDebug("On packet: ", numFramesRemaining, ": ", packet.time);
//         }
//         else {
//             stream.stop();
//         }
//     }
// };


#endif // HAVE_FFMPEG


// =============================================================================
// Realtime Media Queue Test
//
struct MockMediaPacketSource : public PacketSource
    , public basic::Startable
{
    Thread runner;
    PacketSignal emitter;
    int numFramesRemaining = kNumberFramesWanted;

    MockMediaPacketSource()
        : PacketSource(emitter)
    {
        runner.setRepeating(true);
    }

    void start()
    {
        LDebug("Start");
        runner.start([](void* arg) {
            auto self = reinterpret_cast<MockMediaPacketSource*>(arg);
            if (self->numFramesRemaining) {
                self->numFramesRemaining--;

                // 5 mil is 5 seconds of audio time
                std::random_device rd;
                std::mt19937 rng(rd());
                std::uniform_int_distribution<int> uni(1000, 1000000 * 5);
                auto time = uni(rng);
                // LDebug("Emitting: ", self->numFramesRemaining, ": ", time);
                uint8_t buf[10] = {};
                av::MediaPacket p(static_cast<const uint8_t*>(buf), 10, time);
                self->emitter.emit(p);
            }
        },
                     this);
    }

    void stop()
    {
        LDebug("Stop");
        runner.cancel();
    }

    void onStreamStateChange(const PacketStreamState& state)
    {
        LDebug("Stream state: ", state);

        if (state.equals(PacketStreamState::Closed) ||
            state.equals(PacketStreamState::Error)) {
            runner.cancel();
            runner.join();
        }
    }
};


class RealtimeMediaQueueTest : public Test
{
    PacketStream stream;
    int numFramesRemaining = kNumberFramesWanted;

    void run()
    {
        // Create the multiplex encoder
        // auto queue(std::make_shared<av::RealtimePacketQueue<>());

        stream.attachSource(new MockMediaPacketSource, true, true);
        stream.attach(std::make_shared<av::RealtimePacketQueue<av::MediaPacket>>(), 5);
        stream.start();
        stream.emitter += packetSlot(this, &RealtimeMediaQueueTest::onPacketPlayout);

        auto startTime = std::chrono::steady_clock::now();

        while (numFramesRemaining > 0) {
            // LDebug("Waiting for completion: ", numFramesRemaining);
            scy::sleep(10);
        }

        // Verify stream ran for a non-zero duration
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        expect(elapsedMs > 0);
        expect(numFramesRemaining == 0);
    }

    void onPacketPlayout(av::MediaPacket& packet)
    {
        // auto pkt = reinterpret_cast<av::MediaPacket*>(packet);
        if (numFramesRemaining) {
            numFramesRemaining--;
            LDebug("On packet: ", numFramesRemaining, ": ", packet.time);
        } else {
            stream.stop();
        }
    }
};


// class Tests
// {
// public:
//     Application& app;
//
//     Tests(Application& app) : app(app)
//     {
//         LDebug("Running tests...");
//
//         try {
//
//             runMediaCaptureRecorderTest();
// #if 0
//             runAudioRecorderTest();
//             testVideoThumbnailer();
//             runVideoCaptureThreadTest();
//             testVideoCapture();
//             testVideoCaptureStream();
//
//             MediaFactory::instance().devices().print(SDebug);
//             audioCapture = new AudioCapture(device.id, 2, 44100);
//
//             videoCapture = new VideoCapture(0, true);
//             audioCapture = new AudioCapture(0, 1, 16000);
//             audioCapture = new AudioCapture(0, 1, 11025);
//             for (int i = 0; i < 100; i++) { //0000000000
//                 runAudioCaptureTest();
//             }
//
//             testDeviceManager();
//             runAudioCaptureThreadTest();
//             runAudioCaptureTest();
//             runOpenCVMJPEGTest();
//             runVideoRecorderTest();
//             runCaptureRecorderTest();
//             runMultiplexEncoderTest();
//             runStreamEncoderTest();
//             runMediaSocketTest();
//             runFormatFactoryTest();
//             runMotionDetectorTest();
//             runBuildXMLString();
//
//             runStreamEncoderTest();
//             runOpenCVCaptureTest();
//             runDirectShowCaptureTest();
// #endif
//         }
//         catch (std::exception& exc) {
//             LError("Error: ", exc.what());
//         }
//     };
//
//
//
//
//     // ---------------------------------------------------------------------
//     // Video Capture Test
//     //
//     void onVideoCaptureStreamFrame(void* sender, av::MatrixPacket& packet)
//     {
//         LDebug("On stream packet: ", packet.size());
//     }
//
//
//     // ---------------------------------------------------------------------
//     // Video Capture Thread Test
//     //
//     class VideoCaptureThread: public basic::Runnable
//     {
//     public:
//         VideoCaptureThread(int deviceID, const std::string& name = "Capture
//         Thread") :
//             _deviceID(deviceID),
//             _name(name),
//             frames(0),
//             closed(false)
//         {
//             //_thread.setName(name);
//             _thread.start(*this);
//         }
//
//         VideoCaptureThread(const std::string& filename, const std::string&
//         name = "Capture Thread") :
//             _deviceID(0),
//             _filename(filename),
//             _name(name),
//             frames(0),
//             closed(false)
//         {
//             //_thread.setName(name);
//             _thread.start(*this);
//         }
//
//         ~VideoCaptureThread()
//         {
//             closed = true;
//             _thread.join();
//         }
//
//         void run()
//         {
// #if 0
//             VideoCapture::Ptr capture = !_filename.empty() ?
//                 MediaFactory::instance().createFileCapture(_filename) :
//                 MediaFactory::instance().createVideoCapture(_deviceID);
// #endif
//
//             // Initialize the VideoCapture inside the thread context.
//             // This will fail in windows unless the VideoCapture was
//             // previously initialized in the main thread.
//             VideoCapture::Ptr capture(!_filename.empty() ?
//                 new VideoCapture(_filename) :
//                 new VideoCapture(_deviceID));
//
//             capture.emitter += packetSlot(this,
//             &VideoCaptureThread::onVideo);
//             capture.start();
//
//             while (!closed) {
//                 cv::waitKey(5);
//             }
//
//             capture.emitter -= packetSlot(this,
//             &VideoCaptureThread::onVideo);
//             capture.stop();
//
//             LDebug(" Ended..................");
//         }
//
//         void onVideo(void* sender, MatrixPacket& packet)
//         {
//             LDebug("On thread frame: ", packet.size());
//             cv::imshow(_name, *packet.mat);
//             frames++;
//         }
//
//         Thread _thread;
//         int _deviceID;
//         std::string _filename;
//         std::string _name;
//         int frames;
//         bool closed;
//     };
//
//
//     void runVideoCaptureThreadTest()
//     {
//         LDebug("Running video capture test...");
//
//
//         // start and destroy multiple receivers
//         std::list<VideoCaptureThread*> threads;
//
//         try {
//             for (int i = 0; i < 3; i++) {
//                 threads.push_back(new VideoCaptureThread(0));
//             }
//
//             LDebug(" Ending..................");
//         }
//         catch (std::exception& exc) {
//             LError("[VideoCaptureThread] Error: ", exc.what());
//         }
//
//         std::puts("Press any key to continue...");
//         std::getchar();
//         //scy::pause();
//
//         util::clearList(threads);
//     }
//
//
//     // ---------------------------------------------------------------------
//     // Video Thembnailer Test
//     //
//     void testVideoThumbnailer()
//     {
//         LDebug("Starting");
//
//         try {
//             assert(0 && "fixme");
//             //Thumbnailer thumb;
//             //thumb.init("D://test1.mp4", "resto.jpg", 0, 0, 3.0);
//             //thumb.grab();
//         }
//         catch (std::exception& exc) {
//             LError("VideoThumbnailer: ", exc.what());
//         }
//         catch (...) {
//             LError("VideoThumbnailer: Unknown error");
//         }
//
//         LDebug("Complete");
//     }
//
//
// #if 0
//     bool PullFrame(const std::string& capturedUrl)
// {
//     AVCodec* pMJPEGCodec  = avcodec_find_encoder(AV_CODEC_ID_MJPEG );
//
//     int videoStream   = -1;
//
//     AVDictionary *optionsDict = NULL;
//     AVInputFormat   *pFormat = NULL;
//     const char      formatName[] = "mp4";
//
//     if (!(pFormat = av_find_input_format(formatName)))
//     {
//         std::SDebug << "can't find input format " << formatName << "\n";
//         return false;
//     }
//
//     AVFormatContextHandle FormatCtx(avformat_alloc_context());
//
//     if(!FormatCtx.is_valid())
//     {
//         std::SDebug << "\n NULL CONTEXT \n ";
//         return false;
//     }
//
//     if(avformat_open_input (&FormatCtx, capturedUrl.c_str(), pFormat, NULL))
//         return false;
//
//     for(int i=0; i<(int)FormatCtx->nb_streams; i++)
//     {
//         if(FormatCtx->streams[i]->codeencoder.ctx->codec_type==AVMEDIA_TYPE_VIDEO)
//         {
//             videoStream=i;
//             break;
//         }
//     }
//     if(videoStream < 0 )
//         return false;
//
//     CodecContextHandle CodecCtx(FormatCtx->streams[videoStream]->codec,
//     avcodec_close);
//     AVCodec *pCodec = avcodec_find_decoder(CodecCtx->codec_id);
//
//     if(pCodec == NULL)
//         return false;
//
//     if( avcodec_open2(CodecCtx, pCodec, &optionsDict) < 0 )
//         return false;
//
//     FrameHandle Frame(av_frame_alloc(), av_free);
//
//     if(!Frame.is_valid())
//         return false;
//
//     int frameFinished=0;
//     AVPacket packet;
//
//     while(av_read_frame(FormatCtx, &packet)>=0)
//     {
//         if(packet.stream_index==videoStream)
//         {
//             avcodec_decode_video2(CodecCtx, Frame, &frameFinished, &packet);
//
//             if(frameFinished)
//             {
//                 std::string uu (capturedUrl);
//                 size_t pos = capturedUrl.rfind(".mp4");
//                 uu.replace(pos, 4, "thumbnail.jpg");
//                 // save the frame to file
//                 int Bytes = avpicture_get_size(AV_PIX_FMT_YUVJ420P,
//                 CodecCtx->width, CodecCtx->height);
//                 BufferHandle
//                 buffer((uint8_t*)av_malloc(Bytes*sizeof(uint8_t)), av_free);
//                 CodecContextHandle OutContext(avcodec_alloc_context3(NULL),
//                 free_context);
//                 OutContext->bit_rate = CodecCtx->bit_rate;
//                 OutContext->width = CodecCtx->width;
//                 OutContext->height = CodecCtx->height;
//                 OutContext->pix_fmt = AV_PIX_FMT_YUVJ420P;
//                 OutContext->codec_id = AV_CODEC_ID_MJPEG;
//                 OutContext->codec_type = AVMEDIA_TYPE_VIDEO;
//                 OutContext->time_base.num = CodecCtx->time_base.num;
//                 OutContext->time_base.den = CodecCtx->time_base.den;
//                 AVCodec *OutCodec =
//                 avcodec_find_encoder(OutContext->codec_id);
//                 avcodec_open2(OutContext, OutCodec, NULL);
//                 OutContext->mb_lmin = OutContext->lmin = OutContext->qmin *
//                 118;
//                 OutContext->mb_lmax = OutContext->lmax = OutContext->qmax *
//                 118;
//                 OutContext->flags = 2;
//                 OutContext->global_quality = OutContext->qmin * 118;
//
//                 Frame->pts = 1;
//                 Frame->quality = OutContext->global_quality;
//
//                 int ActualSize = avcodec_encode_video(OutContext, buffer,
//                 Bytes, Frame);
//                 std::ofstream file("c:\\temp\\output.jpg",
//                 std::ios_base::binary | std::ios_base::out);
//                 file.write((const char*)(uint8_t*)buffer, ActualSize);
//             }
//             if(CodecCtx->refcounted_frames == 1)
//                 av_frame_unref(Frame);
//         }
//         av_free_packet(&packet);
//     }
//
//     return true;
// }
//
//
//     // ---------------------------------------------------------------------
//     // Packet Stream Encoder Test
//     //
//     static bool stopStreamEncoders;
//
//     class StreamEncoderTest
//     {
//     public:
//         StreamEncoderTest(const av::EncoderOptions& opts) :
//             closed(false), options(opts), frames(0), videoCapture(nullptr),
//             audioCapture(nullptr)
//         {
//             //ofile.open("enctest1.mp4", ios::out | ios::binary);
//             //assert(ofile.is_open());
//
//             // Synchronize events and packet output with the default loop
//             //stream.synchronizeOutput(uv::defaultLoop());
//             //stream.setAsyncContext(std::make_shared<Idler>(uv::defaultLoop()));
//
//             // Init captures
//             if (options.oformat.video.enabled) {
//                 LDebug("Video device: ", 0);
//                 videoCapture =
//                 MediaFactory::instance().createVideoCapture(0); //0
//                 //videoCapture =
//                 MediaFactory::instance().createFileCapture("D:/dev/lib/ffmpeg/bin/channel1.avi");
//                 //0
//                 //videoCapture->emitter += packetSlot(this,
//                 &StreamEncoderTest::onVideoCapture);
//                 videoCapture->getEncoderFormat(options.iformat);
//                 stream.attachSource(videoCapture, true, true); //
//
//                 //options.iformat.video.pixelFmt = "yuv420p";
//             }
//             if (options.oformat.audio.enabled) {
//                 Device device;
//                 if
//                 (MediaFactory::instance().devices().getDefaultMicrophone(device))
//                 {
//                     LDebug("Audio device: ", device.id);
//                     audioCapture =
//                     MediaFactory::instance().createAudioCapture(device.id,
//                         options.oformat.audio.channels,
//                         options.oformat.audio.sampleRate);
//                     stream.attachSource(audioCapture, true, true);
//                 }
//                 else assert(0);
//             }
//
//             // Init as async queue for testing
//             //stream.attach(new FPSLimiter(5), 4, true);
//             //stream.attach(new AsyncPacketQueue, 2, true);
//             //stream.attach(new AsyncPacketQueue, 3, true);
//             //stream.attach(new AsyncPacketQueue, 4, true);
//
//             // Init encoder
//             encoder = new MultiplexPacketEncoder(options,
//                 options.oformat.video.enabled &&
//                 options.oformat.audio.enabled);
//             stream.attach(encoder, 5, true);
//
//             // Start the stream
//             stream.emitter += packetSlot(this,
//             &StreamEncoderTest::onVideoEncoded);
//             stream.StateChange += sdelegate(this,
//             &StreamEncoderTest::onStreamStateChange);
//             stream.start();
//         }
//
//         virtual ~StreamEncoderTest()
//         {
//             LDebug("Destroying");
//             close();
//             LDebug("Destroying: OK");
//         }
//
//         void close()
//         {
//             LDebug("########### Closing: ", frames);
//             closed = true;
//
//             // Close the stream
//             // This will flush any queued items
//             //stream.stop();
//             //stream.waitForSync();
//             stream.close();
//
//             // Make sure everything shutdown properly
//             //assert(stream.queue().empty());
//             //assert(encoder->isStopped());
//
//             // Close the output file
//             //ofile.close();
//         }
//
//         void onStreamStateChange(void* sender, PacketStreamState& state,
//         const PacketStreamState& oldState)
//         {
//             SDebug << "########### On stream state change: " << oldState << "
//             => " << state << endl;
//         }
//
//         void onVideoEncoded(void* sender, RawPacket& packet)
//         {
//             SDebug << "########### On packet: " << closed << ":" <<
//             packet.size() << endl;
//             frames++;
//             //assert(!closed);
//             assert(packet.data());
//             assert(packet.size());
//
//             // Do not call stream::close from inside callback
//             //ofile.write(packet.data(), packet.size());
//             //assert(frames <= 3);
//             //if (frames == 20)
//             //    close();
//         }
//
//         void onVideoCapture(void* sender, av::MatrixPacket& packet)
//         {
//             LDebug("On packet: ", packet.size());
//
//             //cv::imshow("StreamEncoderTest", *packet.mat);
//         }
//
//         int frames;
//         bool closed;
//         PacketStream stream;
//         VideoCapture::Ptr videoCapture;
//         AudioCapture::Ptr audioCapture;
//         //AsyncPacketQueue* queue;
//         MultiplexPacketEncoder* encoder;
//         av::EncoderOptions options;
//         //std::ofstream ofile;
//     };
//
//     static void onShutdownSignal(void* opaque)
//     {
//         auto& tests =
//         *reinterpret_cast<std::vector<StreamEncoderTest*>*>(opaque);
//
//         stopStreamEncoders = true;
//
//         for (unsigned i = 0; i < tests.size(); i++) {
//             // Delete the pointer directly to
//             // ensure synchronization is golden.
//             delete tests[i];
//         }
//     }
//
//     void runStreamEncoderTest()
//     {
//         LDebug("Running");
//         try
//         {
//             // Setup encoding format
//             Format mp4(Format("MP4", "mp4",
//                 VideoCodec("MPEG4", "mpeg4", 640, 480, 60),
//                 //VideoCodec("H264", "libx264", 640, 480, 20)//,
//                 //AudioCodec("AAC", "aac", 2, 44100)
//                 //AudioCodec("MP3", "libmp3lame", 1, 8000, 64000)
//                 //AudioCodec("MP3", "libmp3lame", 2, 44100, 64000)
//                 AudioCodec("AC3", "ac3_fixed", 2, 44100, 64000)
//             ));
//
//             Format mp3("MP3", "mp3",
//                 AudioCodec("MP3", "libmp3lame", 1, 8000, 64000));
//
//             //stopStreamEncoders = false;
//
//             av::EncoderOptions options;
//             //options.ofile = "enctest.mp3";
//             options.ofile = "itsanewday.mp4";
//             //options.ofile = "enctest.mjpeg";
//             options.oformat = mp4;
//
//             // Initialize test runners
//             int numTests = 1;
//             std::vector<StreamEncoderTest*> threads;
//             for (unsigned i = 0; i < numTests; i++) {
//                 threads.push_back(new StreamEncoderTest(options));
//             }
//
//             // Run until Ctrl-C is pressed
//             //Application app;
//             app.waitForShutdown(); //&threadsnullptr, nullptr
//
//             for (unsigned i = 0; i < threads.size(); i++) {
//                 // Delete the pointer directly to
//                 // ensure synchronization is golden.
//                 delete threads[i];
//             }
//
//             // Shutdown the garbage collector so we can free memory.
//             //GarbageCollector::instance().shutdown();
//             //LDebug("#################### Finalizing");
//             //GarbageCollector::instance().shutdown();
//             //LDebug("#################### Exiting");
//
//             //LDebug("#################### Finalizing");
//             //app.cleanup();
//             //LDebug("#################### Exiting");
//
//             // Wait for enter keypress
//             //scy::pause();
//
//             // Finalize the application to free all memory
//             // Note: 2 tiny mem leaks (964 bytes) are from OpenCV
//             //app.finalize();
//         }
//         catch (std::exception& exc)
//         {
//             LError("Error: ", exc.what());
//             assert(0);
//         }
//
//         LDebug("Ended");
//     }
//
//
//     void runCaptureRecorderTest()
//     {
//         LDebug("Starting");
//
//         /*
//         av::VideoCapture::Ptr capture(0);
//
//         // Setup encoding format
//         Format mp4(Format("MP4", "mp4",
//             VideoCodec("MPEG4", "mpeg4", 640, 480, 10)//,
//             //VideoCodec("H264", "libx264", 320, 240, 25),
//             //AudioCodec("AAC", "aac", 2, 44100)
//             //AudioCodec("MP3", "libmp3lame", 2, 44100, 64000)
//             //AudioCodec("AC3", "ac3_fixed", 2, 44100, 64000)
//         ));
//
//         av::EncoderOptions options;
//         options.ofile = "enctest.mp4"; // enctest.mjpeg
//         options.oformat = mp4;
//         setVideoCaptureInputFormat(&capture, options.iformat);
//
//         CaptureRecorder encoder(&capture, options);
//         encoder.start();
//
//         std::puts("Press any key to continue...");
//         std::getchar();
//
//         encoder.stop();
//         */
//
//         LDebug("Complete");
//     }
//
//
//     // ---------------------------------------------------------------------
//     // OpenCV Capture Test
//     //
//     void runOpenCVCaptureTest()
//     {
//         LDebug("Starting");
//
//         cv::VideoCapture cap(0);
//         if (!cap.isOpened())
//             assert(false);
//
//         cv::Mat edges;
//         cv::namedWindow("edges",1);
//         for(;;) {
//             cv::Mat frame;
//             cap >> frame; // get a new frame from camera
//             cv::cvtColor(frame, edges, CV_BGR2GRAY);
//             cv::GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
//             cv::Canny(edges, edges, 0, 30, 3);
//             cv::imshow("edges", edges);
//             if (cv::waitKey(30) >= 0) break;
//         }
//
//         LDebug("Complete");
//     }
//
//
//     // ---------------------------------------------------------------------
//     // Audio Capture Thread Test
//     //
//     class AudioCaptureThread: public basic::Runnable
//     {
//     public:
//         AudioCaptureThread(const std::string& name = "Capture Thread")
//         {
//             _thread.setName(name);
//             _thread.start(*this);
//         }
//
//         ~AudioCaptureThread()
//         {
//             _wakeUp.set();
//             _thread.join();
//         }
//
//         void run()
//         {
//             try
//             {
//                 //capture = new AudioCapture(0, 2, 44100);
//                 //capture = new AudioCapture(0, 1, 16000);
//                 //capture = new AudioCapture(0, 1, 11025);
//                 AudioCapture* capture = new AudioCapture(0, 1, 16000);
//                 capture.attach(audioDelegate(this,
//                 &AudioCaptureThread::onAudioCaptured));
//
//                 _wakeUp.wait();
//
//                 capture.detach(audioDelegate(this,
//                 &AudioCaptureThread::onAudioCaptured));
//                 delete capture;
//
//                 SDebug << "[AudioCaptureThread] Ending.................." <<
//                 endl;
//             }
//             catch (std::exception& exc)
//             {
//                 SError << "[AudioCaptureThread] Error: " << exc.what() <<
//                 endl;
//             }
//
//             LDebug("[AudioCaptureThread] Ended..................");
//             //delete this;
//         }
//
//         void onAudioCaptured(void* sender, AudioPacket& packet)
//         {
//             SDebug << "[AudioCaptureThread] On Packet: " << packet.size() <<
//             endl;
//             //cv::imshow(_thread.name(), *packet.mat);
//         }
//
//         Thread    _thread;
//         Poco::Event        _wakeUp;
//         int                frames;
//     };
//
//
//     void runAudioCaptureThreadTest()
//     {
//         LDebug("Running Audio Capture Thread test...");
//
//         // start and destroy multiple receivers
//         list<AudioCaptureThread*> threads;
//         for (int i = 0; i < 10; i++) { //0000000000
//             threads.push_back(new AudioCaptureThread());
//             //Poco::format("Audio Capture Thread %d", i))
//         }
//
//         //scy::pause();
//
//         util::clearList(threads);
//     }
//
//
//     // ---------------------------------------------------------------------
//     // Audio Capture Test
//     //
//     void onCaptureTestAudioCapture(void*, AudioPacket& packet)
//     {
//         LDebug("onCaptureTestAudioCapture: ", packet.size());
//         //cv::imshow("Target", *packet.mat);
//     }
//
//     void runAudioCaptureTest()
//     {
//         LDebug("Running Audio Capture test...");
//
//         AudioCapture* capture = new AudioCapture(0, 1, 16000);
//         capture.attach(packetSlot(this, &Tests::onCaptureTestAudioCapture));
//         //scy::pause();
//         capture.detach(packetSlot(this, &Tests::onCaptureTestAudioCapture));
//         delete capture;
//
//         AudioCapture* capture = new AudioCapture(0, 1, 16000);
//         capture.attach(packetSlot(this, &Tests::onCaptureTestAudioCapture));
//         //audioCapture->start();
//
//         //scy::pause();
//
//         capture.detach(packetSlot(this, &Tests::onCaptureTestAudioCapture));
//         delete capture;
//         //audioCapture->stop();
//     }
//
//
//     // ---------------------------------------------------------------------
//     //
//     // Video Media Socket Test
//     //
//     // ---------------------------------------------------------------------
//
//     class MediaConnection: public TCPServerConnection
//     {
//     public:
//         MediaConnection(const StreamSocket& s) :
//           TCPServerConnection(s), stop(false)//, lastTimestamp(0),
//           timestampGap(0), waitForKeyFrame(true)
//         {
//         }
//
//         void run()
//         {
//             try
//             {
//                 av::EncoderOptions options;
//                 //options.ofile = "enctest.mp4";
//                 //options.stopAt = time(0) + 3;
//                 av::setVideoCaptureInputForma(videoCapture, options.iformat);
//                 //options.oformat = Format("MJPEG", "mjpeg",
//                 VideoCodec(Codec::MJPEG, "MJPEG", 1024, 768, 25));
//                 //options.oformat = Format("FLV", "flv",
//                 VideoCodec(Codec::H264, "H264", 400, 300, 25));
//                 options.oformat = Format("FLV", "flv", VideoCodec(Codec::FLV,
//                 "FLV", 640, 480, 100));
//                 //options.oformat = Format("FLV", "flv",
//                 VideoCodec(Codec::FLV, "FLV", 320, 240, 15));
//                 //options.oformat = Format("FLV", "flv",
//                 VideoCodec(Codec::H264, "H264", 400, 300, 25));
//
//
//                 //options.iformat.video.pixfmt =
//                 (scy::av::PixelFormat::ID)AV_PIX_FMT_GRAY8;
//                 //AV_PIX_FMT_BGR8; //AV_PIX_FMT_BGR32 // AV_PIX_FMT_BGR32
//                 //MotionDetector* detector = new MotionDetector();
//                 //detector->setVideoCapture(videoCapture);
//                 //stream.attach(detector, true);
//                 //stream.attach(new SurveillanceMJPEGPacketizer(*detector),
//                 20, true);
//
//                 stream.attach(videoCapture, false);
//
//                 stream.attach(packetSlot(this,
//                 &MediaConnection::onVideoEncoded));
//
//                 // init encoder
//                 MultiplexEncoder* encoder = new MultiplexEncoder();
//                 encoder->setParams(options);
//                 encoder->init();
//                 stream.attach(encoder, 5, true);
//
//                 //HTTPMultipartAdapter* packetizer = new
//                 HTTPMultipartAdapter("image/jpeg");
//                 //stream.attach(packetizer);
//
//                 //FLVMetadataInjector* injector = new
//                 FLVMetadataInjector(options.oformat);
//                 //stream.attach(injector);
//
//                 // start the stream
//                 stream.start();
//
//                 while (!stop)
//                 {
//                     Thread::sleep(50);
//                 }
//
//                 //stream.detach(packetSlot(this,
//                 &MediaConnection::onVideoEncoded));
//                 //stream.stop();
//
//                 //outputFile.close();
//                 cerr << "MediaConnection: ENDING!!!!!!!!!!!!!" << endl;
//             }
//             catch (std::exception& exc)
//             {
//                 cerr << "MediaConnection: " << exc.what() << endl;
//             }
//         }
//
//         void onVideoEncoded(void* sender, RawPacket& packet)
//         {
//             StreamSocket& ss = socket();
//
//             fpsCounter.tick();
//             LDebug("On Video Packet Encoded: ", fpsCounter.fps);
//
//             //if (fpsCounter.frames < 10)
//             //    return;
//             //if (fpsCounter.frames == 10) {
//             //    stream.reset();
//             //    return;
//             //}
//
//             try
//             {
//                 ss.sendBytes(packet.data, packet.size);
//             }
//             catch (std::exception& exc)
//             {
//                 cerr << "MediaConnection: " << exc.what() << endl;
//                 stop = true;
//             }
//         }
//
//         bool stop;
//         PacketStream stream;
//         FPSCounter fpsCounter;
//     };
//
//     void runMediaSocketTest()
//     {
//         LDebug("Running Media Socket Test");
//
//         ServerSocket svs(666);
//         TCPServer srv(new TCPServerConnectionFactoryImpl<MediaConnection>(),
//         svs);
//         srv.start();
//         //scy::pause();
//     }
//
//
//     // ---------------------------------------------------------------------
//     // Video CaptureRecorder Test
//     //
//     //UDPSocket outputSocket;
//
//     void runCaptureRecorderTest()
//     {
//         LDebug("Running Capture Encoder Test");
//
//         av::EncoderOptions options;
//         options.ofile = "enctest.mp4";
//         //options.stopAt = time(0) + 3;
//         av::setVideoCaptureInputForma(videoCapture, options.iformat);
//         //options.oformat = Format("MJPEG", "mjpeg", VideoCodec(Codec::MJPEG,
//         "MJPEG", 400, 300));
//         options.oformat = Format("FLV", "flv", VideoCodec(Codec::H264,
//         "H264", 400, 300, 25));
//         //options.oformat = Format("FLV", "flv", VideoCodec(Codec::FLV,
//         "FLV", 320, 240, 15));
//
//         //CaptureRecorder<VideoEncoder> enc(videoCapture, options);
//         //encoder = new MultiplexEncoder(stream.options());
//         CaptureRecorder enc(options, videoCapture, audioCapture);
//         //enc.init();
//
//         enc.attach(packetSlot(this,
//         &Tests::onCaptureRecorderTestVideoEncoded));
//         enc.start();
//         //scy::pause();
//         enc.stop();
//
//         LDebug("Running Capture Encoder Test: END");
//     }
//
//     FPSCounter fpsCounter;
//     void onCaptureRecorderTestVideoEncoded(void* sender, MediaPacket& packet)
//     {
//         fpsCounter.tick();
//         LDebug("On Video Packet Encoded: ", fpsCounter.fps);
//     }
//
//     // ---------------------------------------------------------------------
//     // Video CaptureRecorder Test
//     //
//     void runVideoRecorderTest()
//     {
//         av::EncoderOptions options;
//         options.ofile = "av_capture_test.flv";
//
//         options.oformat = Format("FLV", "flv",
//             VideoCodec(Codec::FLV, "FLV", 320, 240, 15),
//             //AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 11025),
//             AudioCodec(Codec::Speex, "Speex", 1, 16000)//,
//             //AudioCodec(Codec::Speex, "Speex", 2, 44100)
//             );
//         //options.oformat = Format("MP4", Format::MP4,
//         options.oformat = Format("FLV", "flv",
//             //VideoCodec(Codec::MPEG4, "MPEG4", 640, 480, 25),
//             //VideoCodec(Codec::H264, "H264", 640, 480, 25),
//             VideoCodec(Codec::FLV, "FLV", 640, 480, 25),
//             //AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 11025)
//             //AudioCodec(Codec::Speex, "Speex", 2, 44100)
//             //AudioCodec(Codec::MP3, "MP3", 2, 44100)
//             //AudioCodec(Codec::AAC, "AAC", 2, 44100)
//             AudioCodec(Codec::AAC, "AAC", 1, 11025)
//         );
//
//         options.oformat = Format("M4A", Format::M4A,
//             //AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 44100)
//             AudioCodec(Codec::AAC, "AAC", 2, 44100)
//             //AudioCodec(Codec::AC3, "AC3", 2, 44100)
//         );
//
//
//
//         //options.stopAt = time(0) + 5; // Max 24 hours
//         av::setVideoCaptureInputForma(videoCapture, options.iformat);
//
//         CaptureRecorder enc(options, nullptr, audioCapture); //videoCapture
//
//         audioCapture->start();
//         enc.start();
//         //scy::pause();
//         enc.stop();
//     }
// #endif
//
//
//
//     // ---------------------------------------------------------------------
//     // Capture Encoder Test
//     //
//     class CaptureRecorder {
//     public:
//         CaptureRecorder(ICapture* capture, const av::EncoderOptions& options)
//         :
//             capture(capture), encoder(options), closed(false) {
//             assert(capture);
//             encoder.init();
//         };
//
//         void start()
//         {
//             capture.emitter += packetSlot(this, &CaptureRecorder::onFrame);
//             capture.start();
//             //audioCapture->start();
//         }
//
//         void stop()
//         {
//             capture.emitter -= packetSlot(this, &CaptureRecorder::onFrame);
//             capture.stop();
//             encoder.uninit();
//             closed = true;
//         }
//
//         void onFrame(void* sender, RawPacket& packet)
//         {
//             LDebug("On packet: ", packet.size());
//             assert(!closed);
//             try
//             {
//                 encoder.process(packet);
//             }
//             catch (std::exception& exc)
//             {
//                 LError("Capture Recorder Error: ", exc.what());
//                 stop();
//             }
//         }
//
//         ICapture* capture;
//         MultiplexPacketEncoder encoder;
//         bool closed;
//     };
//
//     // ---------------------------------------------------------------------
//     // Audio CaptureRecorder Test
//     //
//     void runAudioRecorderTest()
//     {
//         av::EncoderOptions options;
//         options.ofile = "audio_test.mp3";
//         //options.stopAt = time(0) + 5;
//         options.oformat = av::Format("MP3", "mp3",
//             av::AudioCodec("MP3", "libmp3lame", 2, 44100, 128000, "s16p"));
//
//         av::Device dev;
//         auto& media = av::MediaFactory::instance();
//         media.devices().getDefaultMicrophone(dev);
//         LInfo("Default audio capture ", dev.id);
//         av::MediaCapture::Ptr audioCapture = media.createAudioCapture(0,
//         //dev.id
//             options.oformat.audio.channels,
//             options.oformat.audio.sampleRate);
//         audioCapture->getEncoderFormat(options.iformat);
//
//         CaptureRecorder enc(audioCapture.get(), options);
//
//         enc.start();
//         scy::pause();
//         enc.stop();
//     }
//
//     // ---------------------------------------------------------------------
//     // Audio CaptureRecorder Test
//     //
//     void runMediaCaptureRecorderTest()
//     {
//         PacketStream stream;
//
//         // Create the Encoder Options
//         av::EncoderOptions options;
//         options.ofile = "audio_test.mp4";
//         options.duration = 5; //time(0) +
//         options.oformat = av::Format("AAC", "aac",
//             av::AudioCodec("AAC", "aac", 2, 44100, 96000, "fltp"));
//             //av::AudioCodec("MP3", "libmp3lame", 2, 44100, 128000, "s16p"));
//
//         // Attach the Audio Capture
//         av::MediaCapture::Ptr reader(new av::MediaCapture());
//         reader->openMicrophone(0,
//             options.oformat.audio.channels,
//             options.oformat.audio.sampleRate);
//         reader->getEncoderFormat(options.iformat);
//
//         // Attach the Audio Capture
//         stream.attachSource<av::MediaCapture>(reader, true);
//
//         // Attach the Audio Encoder
//         auto encoder = new av::MultiplexPacketEncoder(options);
//         encoder->init();
//         stream.attach(encoder, 5, true);
//
//         stream.start();
//         scy::pause();
//         stream.stop();
//     }
//
//
//     // ---------------------------------------------------------------------
//     // Audio CaptureRecorder Test
//     //
//     void runAudioStreamRecorderTest()
//     {
//         PacketStream stream;
//
//         // Create the Encoder Options
//         av::EncoderOptions options;
//         options.ofile = "audio_test.mp3";
//         //options.stopAt = time(0) + 5;
//         options.oformat = av::Format("MP3", "mp3",
//             av::AudioCodec("MP3", "libmp3lame", 2, 44100, 128000, "s16p"));
//
//         // Create the Audio Capture
//         av::Device dev;
//         auto& media = av::MediaFactory::instance();
//         media.devices().getDefaultMicrophone(dev);
//         LInfo("Default audio capture ", dev.id);
//         av::MediaCapture::Ptr audioCapture = media.createAudioCapture(0,
//         //dev.id
//             options.oformat.audio.channels,
//             options.oformat.audio.sampleRate);
//         audioCapture->getEncoderFormat(options.iformat);
//
//         // Attach the Audio Capture
//         stream.attachSource<av::MediaCapture>(audioCapture, true);
//
//         // Attach the Audio Encoder
//         //auto encoder = new av::MultiplexPacketEncoder(options);
//         //encoder->init();
//         //stream.attach(encoder, 5, true);
//
//         //CaptureRecorder enc(audioCapture, options);
//
//         stream.start();
//         scy::pause();
//         stream.stop();
//     }
// };


#ifdef HAVE_FFMPEG

// =============================================================================
// Standalone Video Encoder Test
//
// Generates synthetic YUV420P frames, encodes them with libx264,
// and verifies output packets are produced.
//
class VideoEncoderTest : public Test
{
    int numPacketsEncoded = 0;

    void run()
    {
        av::VideoEncoder encoder;

        encoder.iparams.width = 320;
        encoder.iparams.height = 240;
        encoder.iparams.pixelFmt = "yuv420p";

        encoder.oparams.encoder = "libx264";
        encoder.oparams.pixelFmt = "yuv420p";
        encoder.oparams.width = 320;
        encoder.oparams.height = 240;
        encoder.oparams.fps = 25;
        encoder.oparams.bitRate = 400000;
        encoder.oparams.enabled = true;

        encoder.emitter += packetSlot(this, &VideoEncoderTest::onVideoEncoded);
        encoder.create();
        encoder.open();

        // Generate and encode synthetic frames
        const int numFrames = 50;
        for (int i = 0; i < numFrames; i++) {
            AVFrame* frame = encoder.frame;

            // Fill Y plane with gradient pattern
            for (int y = 0; y < encoder.iparams.height; y++) {
                for (int x = 0; x < encoder.iparams.width; x++) {
                    frame->data[0][y * frame->linesize[0] + x] =
                        static_cast<uint8_t>((x + y + i * 3) % 256);
                }
            }
            // Fill U and V planes
            for (int y = 0; y < encoder.iparams.height / 2; y++) {
                for (int x = 0; x < encoder.iparams.width / 2; x++) {
                    frame->data[1][y * frame->linesize[1] + x] =
                        static_cast<uint8_t>((128 + y + i * 2) % 256);
                    frame->data[2][y * frame->linesize[2] + x] =
                        static_cast<uint8_t>((64 + x + i * 5) % 256);
                }
            }

            frame->pts = i;
            encoder.encode(frame);
        }

        encoder.flush();
        encoder.close();

        LDebug("Video encoder test: encoded ", numPacketsEncoded, " packets from ", numFrames, " frames");
        expect(numPacketsEncoded > 0);
    }

    void onVideoEncoded(av::VideoPacket& packet)
    {
        numPacketsEncoded++;
        expect(packet.size() > 0);
        expect(packet.width > 0);
        expect(packet.height > 0);
    }
};


// =============================================================================
// Standalone Video Decoder Test
//
// Decodes video frames from test.mp4, verifies frame dimensions and count.
//
class VideoDecoderTest : public Test
{
    int numFramesDecoded = 0;

    void run()
    {
        std::string inputFile = sampleDataDir("test.mp4");

        // Open the file and find the video stream
        AVFormatContext* formatCtx = nullptr;
        if (avformat_open_input(&formatCtx, inputFile.c_str(), nullptr, nullptr) < 0) {
            std::cout << "Cannot open " << inputFile << ", skipping..." << endl;
            return;
        }
        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            avformat_close_input(&formatCtx);
            std::cout << "Cannot find stream info, skipping..." << endl;
            return;
        }

        // Find video stream
        AVStream* videoStream = nullptr;
        for (unsigned i = 0; i < formatCtx->nb_streams; i++) {
            if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = formatCtx->streams[i];
                break;
            }
        }
        expect(videoStream != nullptr);

        // Create decoder
        av::VideoDecoder decoder(videoStream);
        decoder.emitter += packetSlot(this, &VideoDecoderTest::onVideoDecoded);
        decoder.create();
        decoder.open();

        int width = decoder.oparams.width;
        int height = decoder.oparams.height;
        expect(width > 0);
        expect(height > 0);

        // Decode all video packets
        AVPacket* pkt = av_packet_alloc();
        while (av_read_frame(formatCtx, pkt) >= 0) {
            if (pkt->stream_index == videoStream->index) {
                decoder.decode(*pkt);
            }
            av_packet_unref(pkt);
        }
        decoder.flush();
        av_packet_free(&pkt);

        avformat_close_input(&formatCtx);

        LDebug("Video decoder test: decoded ", numFramesDecoded, " frames (", width, "x", height, ")");
        expect(numFramesDecoded > 0);
    }

    void onVideoDecoded(av::VideoPacket& packet)
    {
        numFramesDecoded++;
        expect(packet.width > 0);
        expect(packet.height > 0);
        expect(packet.size() > 0);
    }
};


// =============================================================================
// Standalone Audio Decoder Test
//
// Decodes audio from test.mp4, verifies sample output.
//
class AudioDecoderTest : public Test
{
    int numFramesDecoded = 0;

    void run()
    {
        std::string inputFile = sampleDataDir("test.mp4");

        // Open the file and find the audio stream
        AVFormatContext* formatCtx = nullptr;
        if (avformat_open_input(&formatCtx, inputFile.c_str(), nullptr, nullptr) < 0) {
            std::cout << "Cannot open " << inputFile << ", skipping..." << endl;
            return;
        }
        if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
            avformat_close_input(&formatCtx);
            std::cout << "Cannot find stream info, skipping..." << endl;
            return;
        }

        // Find audio stream
        AVStream* audioStream = nullptr;
        for (unsigned i = 0; i < formatCtx->nb_streams; i++) {
            if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStream = formatCtx->streams[i];
                break;
            }
        }

        if (!audioStream) {
            avformat_close_input(&formatCtx);
            std::cout << "No audio stream in test file, skipping..." << endl;
            return;
        }

        // Create decoder
        av::AudioDecoder decoder(audioStream);
        decoder.emitter += packetSlot(this, &AudioDecoderTest::onAudioDecoded);
        decoder.create();
        decoder.open();

        expect(decoder.oparams.channels > 0);
        expect(decoder.oparams.sampleRate > 0);

        // Decode all audio packets
        AVPacket* pkt = av_packet_alloc();
        while (av_read_frame(formatCtx, pkt) >= 0) {
            if (pkt->stream_index == audioStream->index) {
                decoder.decode(*pkt);
            }
            av_packet_unref(pkt);
        }
        decoder.flush();
        av_packet_free(&pkt);

        avformat_close_input(&formatCtx);

        LDebug("Audio decoder test: decoded ", numFramesDecoded, " frames");
        expect(numFramesDecoded > 0);
    }

    void onAudioDecoded(av::AudioPacket& packet)
    {
        numFramesDecoded++;
        expect(packet.size() > 0);
    }
};

#endif // HAVE_FFMPEG


} // namespace scy


#endif // SCY_AV_Tests_H


/// @\}
