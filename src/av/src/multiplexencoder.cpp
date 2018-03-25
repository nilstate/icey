///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/multiplexencoder.h"

#ifdef HAVE_FFMPEG

#include "assert.h"

extern "C" {
#include "libavutil/time.h" // av_gettime (deprecated)
}

#ifdef SCY_WIN
#define snprintf _snprintf
#endif


using std::endl;


namespace scy {
namespace av {


MultiplexEncoder::MultiplexEncoder(const EncoderOptions& options)
    : _options(options)
    , _formatCtx(nullptr)
    , _video(nullptr)
    , _audio(nullptr)
    , _ioCtx(nullptr)
    , _ioBuffer(nullptr)
    , _pts(0)
{
    LTrace("Create")
    initializeFFmpeg();
}


MultiplexEncoder::~MultiplexEncoder()
{
    LTrace("Destroy")
    uninit();
    uninitializeFFmpeg();
}


static int dispatchOutputPacket(void* opaque, uint8_t* buffer, int bufferSize)
{
    // Callback example at:
    // http://lists.mplayerhq.hu/pipermail/libav-client/2009-May/003034.html
    auto klass = reinterpret_cast<MultiplexEncoder*>(opaque);
    if (klass) {
        LTrace("Dispatching packet: ", bufferSize)
        if (!klass->isActive()) {
            LWarn("Dropping packet: " ,  bufferSize,  ": ", klass->state())
            return bufferSize;
        }
        MediaPacket packet(buffer, bufferSize);
        klass->emitter.emit(packet);
        LTrace("Dispatching packet: OK: ", bufferSize)
    }

    return bufferSize;
}


void MultiplexEncoder::init()
{
    assert(!isActive());

    STrace << "Initialize:"
                 << "\n\tInput Format: " << _options.iformat.toString()
                 << "\n\tOutput Format: " << _options.oformat.toString()
                 << "\n\tDuration: " << _options.duration << endl;

    try {
        _options.oformat.video.enabled = _options.iformat.video.enabled;
        _options.oformat.audio.enabled = _options.iformat.audio.enabled;
        if (!_options.oformat.video.enabled && !_options.oformat.audio.enabled)
            throw std::runtime_error("Either video or audio parameters must be specified.");

        if (_options.oformat.id.empty())
            throw std::runtime_error("The output container format must be specified.");

        // Allocate the output media context
        assert(!_formatCtx);
        _formatCtx = avformat_alloc_context();
        if (!_formatCtx)
            throw std::runtime_error("Cannot allocate format context.");

        if (!_options.ofile.empty())
            snprintf(_formatCtx->filename, sizeof(_formatCtx->filename), "%s", _options.ofile.c_str());

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
            int ioBufferSize(MAX_VIDEO_PACKET_SIZE); // * 10
            _ioBuffer = new unsigned char[ioBufferSize];
            _ioCtx = avio_alloc_context(_ioBuffer, ioBufferSize,
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
                // if (url_fopen(&_formatCtx->pb, _options.ofile.c_str(), URL_WRONLY) < 0) {
                if (avio_open(&_formatCtx->pb, _options.ofile.c_str(), AVIO_FLAG_WRITE) < 0) {
                    throw std::runtime_error("AVWriter: Unable to open the output file");
                }
            }
        }

        // Write the stream header (if any)
        // TODO: After Ready state
        avformat_write_header(_formatCtx, nullptr);

        // Send the format information to sdout
        av_dump_format(_formatCtx, 0, _options.ofile.c_str(), 1);

        // Get realtime presentation timestamp
        _formatCtx->start_time_realtime = av_gettime();

        setState(this, EncoderState::Ready);
    } catch (std::exception& exc) {
        LError("Error: ", exc.what())
        setState(this, EncoderState::Error); //, exc.what()
        cleanup();
        throw exc;
    }

    LTrace("Initialize: OK")
}


void MultiplexEncoder::uninit()
{
    LTrace("Uninitialize")

    // Write the trailer and dispatch the tail packet if any
    if (_formatCtx && _formatCtx->pb)
        av_write_trailer(_formatCtx);

    LTrace("Uninitializing: Wrote trailer")

    // Free memory
    cleanup();
    setState(this, EncoderState::Stopped);

    LTrace("Uninitialize: OK")
}


void MultiplexEncoder::cleanup()
{
    LTrace("Cleanup")

    // Delete stream encoders
    freeVideo();
    freeAudio();

    // Close the format
    if (_formatCtx) {

        // Free all remaining streams
        for (unsigned int i = 0; i < _formatCtx->nb_streams; i++) {
            av_freep(&_formatCtx->streams[i]->codec);
            av_freep(&_formatCtx->streams[i]);
        }

        // Close the output file (if any)
        if (!_options.ofile.empty() && _formatCtx->pb && _formatCtx->oformat &&
            !(_formatCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(_formatCtx->pb);
        // avio_url_fclose(_formatCtx->pb);

        // Free the format context
        av_free(_formatCtx);
        _formatCtx = nullptr;
    }

    if (_ioBuffer) {
        delete _ioBuffer;
        _ioBuffer = nullptr;
    }

    LTrace("Cleanup: OK")
}


void MultiplexEncoder::flush()
{
    LTrace("Flushing")

    if (_video) {
        _video->flush();
    }
    if (_audio) {
        _audio->flush();
    }
}


EncoderOptions& MultiplexEncoder::options()
{
    return _options;
}


VideoEncoder* MultiplexEncoder::video()
{
    return _video;
}


AudioEncoder* MultiplexEncoder::audio()
{
    return _audio;
}


//
// Helpers
//


// Write a packet to the output stream.
bool MultiplexEncoder::writeOutputPacket(AVPacket& packet)
{
    assert(packet.data);
    assert(packet.size);
    assert(packet.pts != AV_NOPTS_VALUE);
    assert(isActive());

    STrace << "Writing packet:"
           << "\n\tPacket Size: " << packet.size
           << "\n\tPTS: " << packet.pts
           << "\n\tDTS: " << packet.dts
           << "\n\tDuration: " << packet.duration
           << endl;

    // Write the encoded frame to the output file
    if (av_interleaved_write_frame(_formatCtx, &packet) != 0) {
        LWarn("Cannot write packet")
        return false;
    }
    return true;
}


bool MultiplexEncoder::updateStreamPts(AVStream* stream, int64_t* pts)
{
    LTrace("Update PTS: last=",  _pts,  ", input=", *pts)

    // https://docs.thefoundry.co.uk/products/nuke/developers/63/ndkdevguide/examples/ffmpegReader.cpp
    // https://ffmpeg.org/doxygen/trunk/doc_2examples_2muxing_8c-example.html
    // https://ffmpeg.org/doxygen/trunk/doc_2examples_2transcoding_8c-example.html
    int64_t next;
    if (*pts == AV_NOPTS_VALUE) {
        // Set a realtime pts value if not specified
        int64_t delta(av_gettime() - _formatCtx->start_time_realtime);
        next = delta * (double)stream->time_base.den / (double)stream->time_base.num / AV_TIME_BASE;
    }
    else {
        // Convert from input microseconds to encoder stream time base
        next = *pts * (double)stream->time_base.den / (double)stream->time_base.num / AV_TIME_BASE;
    }

    if (next < _pts) {
        LWarn("Invalid pts (", next ,") <= last (", _pts, ")")
        return false;
    }
    else if (next == _pts) {
        LWarn("Dropping frame at duplicate PTS: ", next)
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
    assert(!_video);
    assert(_options.oformat.video.enabled);
    assert(_formatCtx->oformat->video_codec != AV_CODEC_ID_NONE);
    _video = new VideoEncoder(_formatCtx);
    _video->emitter.attach(packetSlot(this, &MultiplexEncoder::onVideoEncoded));
    _video->iparams = _options.iformat.video;
    _video->oparams = _options.oformat.video;
    _video->create();
    _video->open();
}


void MultiplexEncoder::freeVideo()
{
    if (_video) {
        delete _video;
        _video = nullptr;
    }
}


bool MultiplexEncoder::encodeVideo(AVFrame* frame)
{
    LTrace("Encoding video: ", frame->pts)

    assert(isActive());
    assert(_video && _video->frame);

    if (!isActive())
        throw std::runtime_error("The encoder is not initialized");

    if (!_video)
        throw std::runtime_error("No video context");

    if (!frame->data || !frame->width || !frame->height)
        throw std::runtime_error("Invalid video frame");

    if (!updateStreamPts(_video->stream, &frame->pts)) {
        return false; // duplicate PTS
    }

    return _video->encode(frame);
}


bool MultiplexEncoder::encodeVideo(uint8_t* data[4], int linesize[4],
                                   int width, int height, int64_t time)
{
    LTrace("Encoding video: ", time)

    assert(isActive());
    assert(_video && _video->frame);
    assert(data[0]);
    assert(linesize[0]);

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
    LTrace("Encoding video: ", time)

    assert(isActive());
    assert(_video && _video->frame);
    assert(buffer);
    assert(bufferSize);

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
    writeOutputPacket(*reinterpret_cast<AVPacket*>(packet.source));
}


//
// Audio stuff
//


void MultiplexEncoder::createAudio()
{
    LTrace("Create Audio")

    assert(!_audio);
    assert(_options.oformat.audio.enabled);
    assert(_formatCtx->oformat->audio_codec != AV_CODEC_ID_NONE);

    _audio = new AudioEncoder(_formatCtx);
    _audio->emitter.attach(packetSlot(this, &MultiplexEncoder::onAudioEncoded));
    _audio->iparams = _options.iformat.audio;
    _audio->oparams = _options.oformat.audio;
    _audio->create();
    _audio->open();
}


void MultiplexEncoder::freeAudio()
{
    if (_audio) {
        delete _audio;
        _audio = nullptr;
    }

    // if (_audioFifo) {
    //     av_fifo_free(_audioFifo);
    //     _audioFifo = nullptr;
    // }

    // if (_audioBuffer) {
    //     av_free(_audioBuffer);
    //     _audioBuffer = nullptr;
    // }
}


bool MultiplexEncoder::encodeAudio(uint8_t* buffer, int numSamples, int64_t time)
{
    LTrace("Encoding audio packet: samples=",  numSamples,  ", time=", time)
    assert(buffer);
    assert(numSamples);

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
    LTrace("Encoding audio packet: samples=",  numSamples,  ", time=", time)
    assert(buffer[0]);
    assert(numSamples);

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
    writeOutputPacket(*reinterpret_cast<AVPacket*>(packet.source));
}


} // namespace av
} // namespace scy


#endif


/// @\}
