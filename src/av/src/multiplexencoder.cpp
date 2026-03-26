///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/multiplexencoder.h"

#ifdef HAVE_FFMPEG

extern "C" {
#include "libavutil/time.h"
}



namespace icy {
namespace av {


MultiplexEncoder::MultiplexEncoder(const EncoderOptions& options)
    : _options(options)
    , _formatCtx(nullptr)
    , _ioCtx(nullptr)
    , _pts(0)
{
    LTrace("Create");
    initializeFFmpeg();
}


MultiplexEncoder::~MultiplexEncoder() noexcept
{
    LTrace("Destroy");
    uninit();
    uninitializeFFmpeg();
}


static int dispatchOutputPacket(void* opaque, uint8_t* buffer, int bufferSize)
{
    auto klass = reinterpret_cast<MultiplexEncoder*>(opaque);
    if (klass) {
        LTrace("Dispatching packet: ", bufferSize);
        if (!klass->isActive()) {
            LWarn("Dropping packet: ", bufferSize, ": ", klass->state());
            return bufferSize;
        }
        MediaPacket packet(buffer, bufferSize);
        klass->emitter.emit(packet);
        LTrace("Dispatching packet: OK: ", bufferSize);
    }

    return bufferSize;
}


void MultiplexEncoder::init()
{
    if (isActive())
        throw std::runtime_error("Encoder already active");

    STrace << "Initialize:"
           << "\n\tInput Format: " << _options.iformat.toString()
           << "\n\tOutput Format: " << _options.oformat.toString()
           << "\n\tDuration: " << _options.duration;

    try {
        _options.oformat.video.enabled = _options.iformat.video.enabled;
        _options.oformat.audio.enabled = _options.iformat.audio.enabled;
        if (!_options.oformat.video.enabled && !_options.oformat.audio.enabled)
            throw std::runtime_error("Either video or audio parameters must be specified.");

        if (_options.oformat.id.empty())
            throw std::runtime_error("The output container format must be specified.");

        // Allocate the output media context
        if (_formatCtx)
            throw std::runtime_error("Format context already allocated");
        _formatCtx = avformat_alloc_context();
        if (!_formatCtx)
            throw std::runtime_error("Cannot allocate format context.");

        if (!_options.ofile.empty())
            _formatCtx->url = av_strdup(_options.ofile.c_str());

        // Set the container codec
        _formatCtx->oformat = av_guess_format(_options.oformat.id.c_str(),
                                              _options.ofile.c_str(), nullptr);
        if (!_formatCtx->oformat)
            throw std::runtime_error("Cannot find suitable encoding format for " + _options.oformat.name);

        // Initialize encoder contexts
        if (_options.oformat.video.enabled)
            createVideo();
        if (_options.oformat.audio.enabled)
            createAudio();

        if (_options.ofile.empty()) {

            // Operating in streaming mode. Generated packets can be
            // obtained by connecting to the outgoing PacketSignal.
            // Setup the output IO context for our output stream.
            int ioBufferSize(MAX_VIDEO_PACKET_SIZE);
            _ioBuffer = std::make_unique<uint8_t[]>(ioBufferSize);
            _ioCtx = avio_alloc_context(_ioBuffer.get(), ioBufferSize,
                                        AVIO_FLAG_WRITE, this, nullptr,
                                        dispatchOutputPacket, nullptr);
            if (!_ioCtx)
                throw std::runtime_error("Out of memory: Cannot allocate avio buffer.");
            _ioCtx->seekable = 0;
            _formatCtx->pb = _ioCtx;
        } else {

            // Operating in file mode.
            // Open the output file...
            if (!(_formatCtx->oformat->flags & AVFMT_NOFILE)) {
                if (avio_open(&_formatCtx->pb, _options.ofile.c_str(), AVIO_FLAG_WRITE) < 0) {
                    throw std::runtime_error("AVWriter: Unable to open the output file");
                }
            }
        }

        // Write the stream header (if any)
        int ret = avformat_write_header(_formatCtx, nullptr);
        if (ret < 0)
            throw std::runtime_error("Cannot write format header: " + averror(ret));

        // Send the format information to sdout
        av_dump_format(_formatCtx, 0, _options.ofile.c_str(), 1);

        // Get realtime presentation timestamp
        _formatCtx->start_time_realtime = av_gettime_relative();

        setState(this, EncoderState::Ready);
    } catch (std::exception& exc) {
        LError("Error: ", exc.what());
        setState(this, EncoderState::Error);
        cleanup();
        throw exc;
    }

    LTrace("Initialize: OK");
}


void MultiplexEncoder::uninit()
{
    LTrace("Uninitialize");

    // Write the trailer and dispatch the tail packet if any
    if (_formatCtx && _formatCtx->pb) {
        int ret = av_write_trailer(_formatCtx);
        if (ret < 0)
            LWarn("Cannot write format trailer: ", averror(ret));
    }

    LTrace("Uninitializing: Wrote trailer");

    // Free memory
    cleanup();
    setState(this, EncoderState::Stopped);

    LTrace("Uninitialize: OK");
}


void MultiplexEncoder::cleanup()
{
    LTrace("Cleanup");

    // Delete stream encoders
    freeVideo();
    freeAudio();

    // Close the format
    if (_formatCtx) {

        // Close the output file (if any)
        if (!_options.ofile.empty() && _formatCtx->pb && _formatCtx->oformat &&
            !(_formatCtx->oformat->flags & AVFMT_NOFILE)) {
            int ret = avio_close(_formatCtx->pb);
            if (ret < 0)
                LWarn("Cannot close output file: ", averror(ret));
        }

        // Free the format context (also frees streams)
        avformat_free_context(_formatCtx);
        _formatCtx = nullptr;
    }

    _ioBuffer.reset();

    LTrace("Cleanup: OK");
}


void MultiplexEncoder::flush()
{
    LTrace("Flushing");

    if (_video) {
        _video->flush();
    }
    if (_audio) {
        _audio->flush();
    }
}


const EncoderOptions& MultiplexEncoder::options() const
{
    return _options;
}


VideoEncoder* MultiplexEncoder::video()
{
    return _video.get();
}


AudioEncoder* MultiplexEncoder::audio()
{
    return _audio.get();
}


//
// Helpers
//


// Write a packet to the output stream.
bool MultiplexEncoder::writeOutputPacket(AVPacket& packet)
{
    if (!packet.data || !packet.size)
        return false;
    if (packet.pts == AV_NOPTS_VALUE)
        return false;
    if (!isActive())
        return false;

    STrace << "Writing packet:"
           << "\n\tPacket Size: " << packet.size
           << "\n\tPTS: " << packet.pts
           << "\n\tDTS: " << packet.dts
           << "\n\tDuration: " << packet.duration
;

    // Write the encoded frame to the output file
    if (av_interleaved_write_frame(_formatCtx, &packet) != 0) {
        LWarn("Cannot write packet");
        return false;
    }
    return true;
}


bool MultiplexEncoder::updateStreamPts(AVStream* stream, int64_t* pts)
{
    std::lock_guard<std::mutex> guard(_mutex);

    LTrace("Update PTS: last=", _pts, ", input=", *pts);

    int64_t next;
    if (*pts == AV_NOPTS_VALUE) {
        // Set a realtime pts value if not specified
        int64_t delta(av_gettime_relative() - _formatCtx->start_time_realtime);
        next = delta * static_cast<double>(stream->time_base.den) / static_cast<double>(stream->time_base.num) / AV_TIME_BASE;
    } else {
        // Convert from input microseconds to encoder stream time base
        next = *pts * static_cast<double>(stream->time_base.den) / static_cast<double>(stream->time_base.num) / AV_TIME_BASE;
    }

    if (next < _pts) {
        LWarn("Invalid pts (", next, ") <= last (", _pts, ")");
        return false;
    } else if (next == _pts) {
        LWarn("Dropping frame at duplicate PTS: ", next);
        return false;
    }

    _pts = *pts = next;
    return true;
}


//
// Video stuff
//


void MultiplexEncoder::createVideo()
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (_video)
        throw std::runtime_error("Video encoder already created");
    if (!_options.oformat.video.enabled)
        throw std::runtime_error("Video output is not enabled");
    if (_formatCtx->oformat->video_codec == AV_CODEC_ID_NONE)
        throw std::runtime_error("No video codec available for this format");
    _video = std::make_unique<VideoEncoder>(_formatCtx);
    (void)_video->emitter.attach(packetSlot(this, &MultiplexEncoder::onVideoEncoded));
    _video->iparams = _options.iformat.video;
    _video->oparams = _options.oformat.video;
    _video->create();
    _video->open();
}


void MultiplexEncoder::freeVideo()
{
    _video.reset();
}


bool MultiplexEncoder::encodeVideo(AVFrame* frame)
{
    LTrace("Encoding video: ", frame->pts);

    if (!isActive())
        throw std::runtime_error("The encoder is not initialized");

    if (!_video)
        throw std::runtime_error("No video context");

    if (!frame->data[0] || !frame->width || !frame->height)
        throw std::runtime_error("Invalid video frame");

    if (!updateStreamPts(_video->stream, &frame->pts)) {
        return false; // duplicate PTS
    }

    return _video->encode(frame);
}


bool MultiplexEncoder::encodeVideo(uint8_t* data[4], int linesize[4],
                                   int width, int height, int64_t time)
{
    LTrace("Encoding video: ", time);

    if (!isActive())
        throw std::runtime_error("The encoder is not initialized");

    if (!_video)
        throw std::runtime_error("No video context");

    if (!data[0] || !linesize[0] || !width || !height)
        throw std::runtime_error("Invalid video frame");

    if (!updateStreamPts(_video->stream, &time)) {
        return false; // duplicate PTS
    }

    return _video->encode(data, linesize, time);
}


bool MultiplexEncoder::encodeVideo(uint8_t* buffer, int bufferSize,
                                   int width, int height, int64_t time)
{
    LTrace("Encoding video: ", time);

    if (!isActive())
        throw std::runtime_error("The encoder is not initialized");

    if (!_video)
        throw std::runtime_error("No video context");

    if (!buffer || !bufferSize || !width || !height)
        throw std::runtime_error("Invalid video frame");

    if (!updateStreamPts(_video->stream, &time)) {
        return false; // duplicate PTS
    }

    return _video->encode(buffer, bufferSize, time);
}


void MultiplexEncoder::onVideoEncoded(av::VideoPacket& packet)
{
    std::lock_guard<std::mutex> guard(_mutex);
    writeOutputPacket(*packet.avpacket);
}


//
// Audio stuff
//


void MultiplexEncoder::createAudio()
{
    std::lock_guard<std::mutex> guard(_mutex);
    LTrace("Create Audio");

    if (_audio)
        throw std::runtime_error("Audio encoder already created");
    if (!_options.oformat.audio.enabled)
        throw std::runtime_error("Audio output is not enabled");
    if (_formatCtx->oformat->audio_codec == AV_CODEC_ID_NONE)
        throw std::runtime_error("No audio codec available for this format");

    _audio = std::make_unique<AudioEncoder>(_formatCtx);
    (void)_audio->emitter.attach(packetSlot(this, &MultiplexEncoder::onAudioEncoded));
    _audio->iparams = _options.iformat.audio;
    _audio->oparams = _options.oformat.audio;
    _audio->create();
    _audio->open();
}


void MultiplexEncoder::freeAudio()
{
    _audio.reset();
}


bool MultiplexEncoder::encodeAudio(uint8_t* buffer, int numSamples, int64_t time)
{
    LTrace("Encoding audio packet: samples=", numSamples, ", time=", time);

    if (!buffer || !numSamples)
        throw std::runtime_error("Invalid audio input");

    if (!isActive())
        throw std::runtime_error("The encoder is not initialized");

    if (!_audio)
        throw std::runtime_error("No audio context");

    updateStreamPts(_audio->stream, &time);

    return _audio->encode(buffer, numSamples, time);
}


bool MultiplexEncoder::encodeAudio(uint8_t* buffer[4], int numSamples, int64_t time)
{
    LTrace("Encoding audio packet: samples=", numSamples, ", time=", time);

    if (!buffer[0] || !numSamples)
        throw std::runtime_error("Invalid audio input");

    if (!isActive())
        throw std::runtime_error("The encoder is not initialized");

    if (!_audio)
        throw std::runtime_error("No audio context");

    updateStreamPts(_audio->stream, &time);

    return _audio->encode(buffer, numSamples, time);
}


void MultiplexEncoder::onAudioEncoded(av::AudioPacket& packet)
{
    std::lock_guard<std::mutex> guard(_mutex);
    writeOutputPacket(*packet.avpacket);
}


} // namespace av
} // namespace icy


#endif


/// @}
