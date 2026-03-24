///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/mediacapture.h"

#ifdef HAVE_FFMPEG

#include "icy/av/devicemanager.h"
#include "icy/logger.h"
#include "icy/platform.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}




namespace icy {
namespace av {


MediaCapture::MediaCapture()
    : _formatCtx(nullptr)
    , _stopping(false)
    , _looping(false)
    , _realtime(false)
    , _ratelimit(false)
{
    initializeFFmpeg();
}


MediaCapture::~MediaCapture() noexcept
{
    close();
    uninitializeFFmpeg();
}


void MediaCapture::close()
{
    LTrace("Closing");

    stop();

    {
        std::lock_guard<std::mutex> guard(_mutex);
        _video.reset();
        _audio.reset();

        if (_formatCtx) {
            avformat_close_input(&_formatCtx);
            _formatCtx = nullptr;
        }
    }

    LTrace("Closing: OK");
}


void MediaCapture::openFile(const std::string& file)
{
    LTrace("Opening file: ", file);
    openStream(file, nullptr, nullptr);
}


void MediaCapture::openStream(const std::string& filename, const AVInputFormat* inputFormat, AVDictionary** formatParams)
{
    LTrace("Opening stream: ", filename);

    if (_formatCtx)
        throw std::runtime_error("Capture has already been initialized");

    if (avformat_open_input(&_formatCtx, filename.c_str(), inputFormat, formatParams) < 0)
        throw std::runtime_error("Cannot open the media source: " + filename);

    if (avformat_find_stream_info(_formatCtx, nullptr) < 0)
        throw std::runtime_error("Cannot find stream information: " + filename);

    av_dump_format(_formatCtx, 0, filename.c_str(), 0);

    for (unsigned i = 0; i < _formatCtx->nb_streams; i++) {
        auto stream = _formatCtx->streams[i];
        if (!_video && stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            _video = std::make_unique<VideoDecoder>(stream);
            (void)_video->emitter.attach(packetSlot(this, &MediaCapture::emit));
            _video->create();
            _video->open();
        } else if (!_audio && stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            _audio = std::make_unique<AudioDecoder>(stream);
            (void)_audio->emitter.attach(packetSlot(this, &MediaCapture::emit));
            _audio->create();
            _audio->open();
        }
    }

    if (!_video && !_audio)
        throw std::runtime_error("Cannot find a valid media stream: " + filename);
}


void MediaCapture::start()
{
    LTrace("Starting");

    std::lock_guard<std::mutex>
        guard(_mutex);

    if (!_video && !_audio)
        throw std::runtime_error("No media streams available");

    if ((_video || _audio) && !_thread.running()) {
        LTrace("Initializing thread");
        _stopping = false;
        _thread.start([this]() { run(); });
    }
}


void MediaCapture::stop()
{
    LTrace("Stopping");

    std::lock_guard<std::mutex>
        guard(_mutex);

    _stopping = true;
    if (_thread.running()) {
        LTrace("Terminating thread");
        _thread.cancel();
        _thread.join();
    }
}


void MediaCapture::emit(IPacket& packet)
{
    LTrace("Emit: ", packet.size());

    emitter.emit(packet);
}


void MediaCapture::run()
{
    LTrace("Running");

    try {
        int res;
        AVPacket* ipacket = av_packet_alloc();
        if (!ipacket)
            throw std::runtime_error("Cannot allocate packet");

        // Looping variables
        int64_t videoPtsOffset = 0;
        int64_t audioPtsOffset = 0;

        // Realtime variables
        int64_t startTime = time::hrtime();

        // Rate limiting variables
        int64_t lastTimestamp = time::hrtime();
        int64_t frameInterval = _video ? fpsToInterval(int(_video->iparams.fps)) : 0;

        // Reset the stream back to the beginning when looping is enabled
        if (_looping) {
            LTrace("Looping");
            for (unsigned i = 0; i < _formatCtx->nb_streams; i++) {
                if (avformat_seek_file(_formatCtx, i, 0, 0, 0, AVSEEK_FLAG_FRAME) < 0) {
                    throw std::runtime_error("Cannot reset media stream");
                }
            }
        }

        // Read input packets until complete
        while ((res = av_read_frame(_formatCtx, ipacket)) >= 0) {
            STrace << "Read frame: "
                   << "pts=" << ipacket->pts << ", "
                   << "dts=" << ipacket->dts;

            if (_stopping)
                break;

            if (_video && ipacket->stream_index == _video->stream->index) {

                // Realtime PTS calculation in microseconds
                if (_realtime) {
                    ipacket->pts = time::hrtime() - startTime;
                } else if (_looping) {
                    // Set the PTS offset when looping
                    if (ipacket->pts == 0 && _video->pts > 0)
                        videoPtsOffset = _video->pts;
                    ipacket->pts += videoPtsOffset;
                }

                // Decode and emit
                if (_video->decode(*ipacket)) {
                    STrace << "Decoded video: "
                           << "time=" << _video->time << ", "
                           << "pts=" << _video->pts;
                }

                // Pause the input stream in rate limited mode if the
                // decoder is working too fast
                if (_ratelimit) {
                    auto nsdelay = frameInterval - (time::hrtime() - lastTimestamp);
                    std::this_thread::sleep_for(std::chrono::nanoseconds(nsdelay));
                    lastTimestamp = time::hrtime();
                }
            } else if (_audio && ipacket->stream_index == _audio->stream->index) {

                // Set the PTS offset when looping
                if (_looping) {
                    if (ipacket->pts == 0 && _audio->pts > 0)
                        audioPtsOffset = _audio->pts;
                    ipacket->pts += audioPtsOffset;
                }

                // Decode and emit
                if (_audio->decode(*ipacket)) {
                    STrace << "Decoded Audio: "
                           << "time=" << _audio->time << ", "
                           << "pts=" << _audio->pts;
                }
            }

            av_packet_unref(ipacket);
        }

        // Flush remaining packets
        if (!_stopping && res < 0) {
            if (_video)
                _video->flush();
            if (_audio)
                _audio->flush();
        }

        av_packet_free(&ipacket);

        // End of file or error
        LTrace("Decoder EOF: ", res);
    } catch (std::exception& exc) {
        _error = exc.what();
        LError("Decoder Error: ", _error);
    } catch (...) {
        _error = "Unknown Error";
        LError("Unknown Error");
    }

    if (_stopping || !_looping) {
        LTrace("Exiting");
        _stopping = true;
        Closing.emit();
    }
}


void MediaCapture::getEncoderFormat(Format& format)
{
    format.name = "Capture";
    getEncoderVideoCodec(format.video);
    getEncoderAudioCodec(format.audio);
}


void MediaCapture::getEncoderAudioCodec(AudioCodec& params)
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (_audio) {
        if (!_audio->oparams.channels || !_audio->oparams.sampleRate || _audio->oparams.sampleFmt.empty())
            throw std::runtime_error("Audio codec parameters not initialized");
        params = _audio->oparams;
    }
}


void MediaCapture::getEncoderVideoCodec(VideoCodec& params)
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (_video) {
        if (!_video->oparams.width || !_video->oparams.height || _video->oparams.pixelFmt.empty())
            throw std::runtime_error("Video codec parameters not initialized");
        params = _video->oparams;
    }
}


AVFormatContext* MediaCapture::formatCtx() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _formatCtx;
}


VideoDecoder* MediaCapture::video() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _video.get();
}


AudioDecoder* MediaCapture::audio() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _audio.get();
}


void MediaCapture::setLoopInput(bool flag)
{
    _thread.setRepeating(flag);
    _looping = flag;
}


void MediaCapture::setLimitFramerate(bool flag)
{
    _ratelimit = flag;
}


void MediaCapture::setRealtimePTS(bool flag)
{
    _realtime = flag;
}


bool MediaCapture::stopping() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _stopping;
}


std::string MediaCapture::error() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _error;
}


} // namespace av
} // namespace icy


#endif


/// @}
