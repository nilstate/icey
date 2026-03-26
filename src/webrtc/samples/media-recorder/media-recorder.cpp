///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Media Recorder
//
// Receives H.264 video from a browser peer via WebRTC, decodes it,
// and records it to an MP4 file using FFmpeg's MultiplexEncoder.
//
// Pipeline:
//   [browser] → WebRtcTrackReceiver → VideoDecoder → MultiplexEncoder → file
//
// Demonstrates the receive + decode + record path.
//
// Usage:
//   media-recorder -output recording.mp4 -host <symple-server> -port <port>
//
/// @addtogroup webrtc
/// @{


#include "icy/application.h"
#include "icy/av/ffmpeg.h"
#include "icy/av/multiplexpacketencoder.h"
#include "icy/av/videodecoder.h"
#include "icy/logger.h"
#include "icy/packetsignal.h"
#include "icy/symple/client.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/webrtc/peersession.h"
#include "icy/webrtc/support/symplesignaller.h"

#include <iostream>
#include <memory>
#include <string>

extern "C" {
#include <libavutil/avutil.h>
}


using namespace icy;


class MediaRecorder
{
public:
    smpl::Client client;
    std::unique_ptr<wrtc::SympleSignaller> signaller;
    std::unique_ptr<wrtc::PeerSession> session;
    std::string outputFile;

    MediaRecorder(const smpl::Client::Options& opts, const std::string& output)
        : client(opts)
        , outputFile(output)
    {
    }

    void start()
    {
        client.Announce += slot(this, &MediaRecorder::onAnnounce);
        client.StateChange += slot(this, &MediaRecorder::onStateChange);
        client.CreatePresence += slot(this, &MediaRecorder::onCreatePresence);

        client.connect();
    }

    void shutdown()
    {
        stopRecording();
        if (session)
            session->hangup("shutdown");
        session.reset();
        client.close();
    }

private:
    void createSession()
    {
        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        // Receive-only: we create tracks to signal we can receive,
        // but we don't send media ourselves.
        config.media.videoCodec = wrtc::CodecNegotiator::resolveWebRtcVideoCodec(
            av::VideoCodec("H264", "libx264", 640, 480, 30));
        config.enableDataChannel = false;

        signaller = std::make_unique<wrtc::SympleSignaller>(client);
        session = std::make_unique<wrtc::PeerSession>(*signaller, config);

        session->IncomingCall += [this](const std::string& peerId) {
            std::cout << "Incoming call from " << peerId << " - accepting" << '\n';
            session->accept();
        };

        session->StateChanged += [this](wrtc::PeerSession::State state) {
            std::cout << "Call state: " << wrtc::stateToString(state) << '\n';

            if (state == wrtc::PeerSession::State::Active) {
                startRecording();
            }
            else if (state == wrtc::PeerSession::State::Ended) {
                stopRecording();
            }
        };
    }

    void startRecording()
    {
        if (!session || _recording)
            return;

        ensureDecoder();
        session->media().videoReceiver().emitter +=
            packetSlot(this, &MediaRecorder::onEncodedVideo);
        _decoder->emitter += packetSlot(this, &MediaRecorder::onDecodedVideo);
        _recording = true;
        std::cout << "Recording to " << outputFile << '\n';
    }

    void stopRecording()
    {
        if (!_recording && !_decoder && !_mux)
            return;

        if (session)
            session->media().videoReceiver().emitter -= this;
        if (_decoder)
            _decoder->emitter -= this;

        bool wroteFile = static_cast<bool>(_mux);

        _mux.reset();
        _decoder.reset();
        _decodeFormat.reset();
        _decodeStream = nullptr;
        _recording = false;
        _waitingForKeyframe = true;
        _loggedWaitingForKeyframe = false;

        if (wroteFile)
            std::cout << "Recording saved to " << outputFile << '\n';
        else
            std::cout << "Call ended before any decodable video frame was recorded\n";
    }

    void ensureDecoder()
    {
        if (_decoder)
            return;

        _decodeFormat.reset(avformat_alloc_context());
        if (!_decodeFormat)
            throw std::runtime_error("Cannot allocate decoder format context");

        _decodeStream = avformat_new_stream(_decodeFormat.get(), nullptr);
        if (!_decodeStream)
            throw std::runtime_error("Cannot allocate decoder stream");

        _decodeStream->time_base = AVRational{1, 90000};
        _decodeStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        _decodeStream->codecpar->codec_id = AV_CODEC_ID_H264;

        _decoder = std::make_unique<av::VideoDecoder>(_decodeStream);
        _decoder->create();
        _decoder->open();
    }

    void ensureMux(const av::PlanarVideoPacket& packet)
    {
        if (_mux)
            return;

        av::EncoderOptions options;
        options.ofile = outputFile;
        options.iformat = av::Format("WebRTC Input", "rawvideo",
            av::VideoCodec("decoded", packet.width, packet.height, 30.0,
                           0, 0, packet.pixelFmt));
        options.oformat = av::Format("MP4", "mp4",
            av::VideoCodec("H264", "libx264", packet.width, packet.height, 30.0));

        _mux = std::make_unique<av::MultiplexPacketEncoder>(options);
        _mux->init();
    }

    void onEncodedVideo(av::VideoPacket& packet)
    {
        if (!_decoder || !_decodeStream)
            return;

        auto ffpacket = av::makeOwnedPacket(packet,
                                            _decodeStream->index,
                                            _decodeStream->time_base);

        try {
            bool decoded = _decoder->decode(*ffpacket);
            if (decoded) {
                _waitingForKeyframe = false;
                _loggedWaitingForKeyframe = false;
            }
        }
        catch (const std::exception&) {
            if (_waitingForKeyframe && !_loggedWaitingForKeyframe) {
                std::cout << "Waiting for a decodable H.264 keyframe..." << '\n';
                _loggedWaitingForKeyframe = true;
            }
        }
    }

    void onDecodedVideo(av::PlanarVideoPacket& packet)
    {
        ensureMux(packet);
        _mux->encode(packet);
    }

    void onAnnounce(const int& status)
    {
        if (status != 200)
            std::cerr << "Auth failed: " << status << '\n';
    }

    void onStateChange(void*, smpl::ClientState& state, const smpl::ClientState&)
    {
        if (state.id() == smpl::ClientState::Online) {
            std::cout << "Online as " << client.ourID() << '\n';
            client.joinRoom("public");
            createSession();
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "Icey";
        peer["type"] = "recorder";
    }

    av::AVFormatContextHolder _decodeFormat;
    AVStream* _decodeStream = nullptr;
    std::unique_ptr<av::VideoDecoder> _decoder;
    std::unique_ptr<av::MultiplexPacketEncoder> _mux;
    bool _recording = false;
    bool _waitingForKeyframe = true;
    bool _loggedWaitingForKeyframe = false;
};


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    smpl::Client::Options opts;
    opts.host = "127.0.0.1";
    opts.port = 4500;
    std::string output = "recording.mp4";

    for (int i = 1; i + 1 < argc; i += 2) {
        std::string key = argv[i];
        std::string val = argv[i + 1];
        if (key == "-host") opts.host = val;
        else if (key == "-port") opts.port = static_cast<uint16_t>(std::stoi(val));
        else if (key == "-token") opts.token = val;
        else if (key == "-user") opts.user = val;
        else if (key == "-name") opts.name = val;
        else if (key == "-output") output = val;
    }

    if (opts.user.empty()) {
        opts.user = "recorder";
        opts.name = "Media Recorder";
    }

    MediaRecorder app(opts, output);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<MediaRecorder*>(opaque)->shutdown();
    }, &app);

    Logger::destroy();
    return 0;
}

/// @}
