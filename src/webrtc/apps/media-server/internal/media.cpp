#include "media.h"

#include "config.h"
#include "../../../src/codecregistry.h"
#include "icy/logger.h"
#include "icy/webrtc/codecnegotiator.h"

#include <chrono>
#include <iostream>


namespace icy {
namespace media_server {


class VideoRecorder
{
public:
    static constexpr size_t MAX_PREROLL_BYTES = 2 * 1024 * 1024;

    VideoRecorder(std::string peerId, std::string recordDir)
        : _peerId(std::move(peerId))
        , _recordDir(std::move(recordDir))
    {
    }

    void arm(wrtc::MediaBridge& media)
    {
        if (_bridge == &media)
            return;
        if (_bridge)
            _bridge->videoReceiver().emitter -= this;
        _bridge = &media;
        _bridge->videoReceiver().emitter +=
            packetSlot(this, &VideoRecorder::onBufferedVideo);
    }

    void start(wrtc::MediaBridge& media)
    {
        if (_recording)
            return;

        arm(media);
        ensureDecoder();
        _outputFile = makeRecordingPath(_recordDir, _peerId);

        _bridge->videoReceiver().emitter +=
            packetSlot(this, &VideoRecorder::onEncodedVideo);
        _decoder->emitter += packetSlot(this, &VideoRecorder::onDecodedVideo);
        _recording = true;

        auto buffered = std::move(_preroll);
        _preroll.clear();
        _prerollBytes = 0;
        for (auto& packet : buffered)
            onEncodedVideo(packet);

        std::cout << "Recording " << _peerId << " to " << _outputFile << '\n';
    }

    void stop()
    {
        if (!_recording && !_bridge && !_decoder && !_mux)
            return;

        if (_bridge) {
            _bridge->videoReceiver().emitter -= this;
            _bridge = nullptr;
        }
        if (_decoder)
            _decoder->emitter -= this;

        const bool wroteFile = static_cast<bool>(_mux);
        const std::string outputFile = _outputFile;

        _mux.reset();
        _decoder.reset();
        _decodeFormat.reset();
        _decodeStream = nullptr;
        _outputFile.clear();
        _inputCodecName.clear();
        _recording = false;
        _waitingForKeyframe = true;
        _loggedWaitingForKeyframe = false;
        _preroll.clear();
        _prerollBytes = 0;

        if (wroteFile)
            std::cout << "Recording saved to " << outputFile << '\n';
        else if (!outputFile.empty())
            std::cout << "Call ended before any decodable video frame was recorded for "
                      << _peerId << '\n';
    }

private:
    void onBufferedVideo(av::VideoPacket& packet)
    {
        if (_recording || packet.size() == 0)
            return;

        _preroll.emplace_back(packet);
        _prerollBytes += packet.size();
        while (_prerollBytes > MAX_PREROLL_BYTES && !_preroll.empty()) {
            _prerollBytes -= _preroll.front().size();
            _preroll.pop_front();
        }
    }

    void ensureDecoder()
    {
        if (_decoder)
            return;
        if (!_bridge)
            throw std::runtime_error("Cannot create recorder decoder without a media bridge");

        auto track = _bridge->videoTrack();
        if (!track)
            throw std::runtime_error("Cannot determine recorder codec before the video track exists");

        auto spec = wrtc::codec_registry::detectInMedia(
            track->description(), wrtc::CodecMediaType::Video);
        if (!spec)
            throw std::runtime_error("Cannot determine negotiated recorder video codec");

        _decodeFormat.reset(avformat_alloc_context());
        if (!_decodeFormat)
            throw std::runtime_error("Cannot allocate decoder format context");

        _decodeStream = avformat_new_stream(_decodeFormat.get(), nullptr);
        if (!_decodeStream)
            throw std::runtime_error("Cannot allocate decoder stream");

        _decodeStream->time_base = AVRational{1, 90000};
        _decodeStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        _decodeStream->codecpar->codec_id = wrtc::codec_registry::decoderCodecId(*spec);
        _inputCodecName = spec->rtpName;

        _decoder = std::make_unique<av::VideoDecoder>(_decodeStream);
        _decoder->create();
        _decoder->open();
    }

    void ensureMux(const av::PlanarVideoPacket& packet)
    {
        if (_mux)
            return;

        av::EncoderOptions options;
        options.ofile = _outputFile;
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
        catch (const std::exception& exc) {
            if (_waitingForKeyframe && !_loggedWaitingForKeyframe) {
                LWarn("Waiting for a decodable ",
                      _inputCodecName.empty() ? std::string("video") : _inputCodecName,
                      " keyframe from ",
                      _peerId,
                      ": ",
                      exc.what());
                _loggedWaitingForKeyframe = true;
            }
        }
    }

    void onDecodedVideo(av::PlanarVideoPacket& packet)
    {
        ensureMux(packet);
        _mux->encode(packet);
    }

    std::string _peerId;
    std::string _recordDir;
    std::string _outputFile;
    std::string _inputCodecName;
    wrtc::MediaBridge* _bridge = nullptr;
    std::deque<av::VideoPacket> _preroll;
    size_t _prerollBytes = 0;
    av::AVFormatContextHolder _decodeFormat;
    AVStream* _decodeStream = nullptr;
    std::unique_ptr<av::VideoDecoder> _decoder;
    std::unique_ptr<av::MultiplexPacketEncoder> _mux;
    bool _recording = false;
    bool _waitingForKeyframe = true;
    bool _loggedWaitingForKeyframe = false;
};


void VideoRecorderDeleter::operator()(VideoRecorder* recorder) const
{
    delete recorder;
}


MediaSession::MediaSession(const std::string& peerId,
                           smpl::Server& server,
                           const std::string& serverPeerId,
                           const Config& config,
                           RelayController* relay)
    : _peerId(peerId)
    , _stream("media-" + peerId)
    , _config(config)
    , _relay(relay)
{
    av::VideoCodec videoCodec = makeVideoCodec(config);
    av::AudioCodec audioCodec = makeAudioCodec(config);

    wrtc::PeerSession::Config pc;
    pc.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");

    if (config.mode == Config::Mode::Stream) {
        pc.media.videoCodec = videoCodec;
        pc.media.audioCodec = audioCodec;
        pc.media.videoDirection = rtc::Description::Direction::SendOnly;
        pc.media.audioDirection = rtc::Description::Direction::SendOnly;
    }
    else if (config.mode == Config::Mode::Record) {
        pc.media.videoCodec = videoCodec;
        pc.media.videoDirection = rtc::Description::Direction::RecvOnly;
        pc.media.audioDirection = rtc::Description::Direction::Inactive;
    }
    else if (config.mode == Config::Mode::Relay) {
        pc.media.videoCodec = videoCodec;
        pc.media.audioCodec = audioCodec;
    }

    pc.enableDataChannel = true;
    pc.dataChannelLabel = "control";

    _signaller = std::make_unique<wrtc::SympleServerSignaller>(server, serverPeerId, peerId);
    _session = std::make_unique<wrtc::PeerSession>(*_signaller, pc);
    if (_config.mode == Config::Mode::Record) {
        _recorder = std::unique_ptr<VideoRecorder, VideoRecorderDeleter>(
            new VideoRecorder(_peerId, _config.recordDir));
        _recorder->arm(_session->media());
    }

    _session->IncomingCall += [this](const std::string& peer) {
        LInfo("Auto-accepting call from ", peer);
        _session->accept();
    };

    _session->StateChanged += [this](wrtc::PeerSession::State state) {
        LInfo("Session ", _peerId, ": ", wrtc::stateToString(state));
        if (state == wrtc::PeerSession::State::Active) {
            startStreaming();
            startRecording();
            startRelay();
            if (_config.mode == Config::Mode::Record)
                _session->media().requestKeyframe();
            if (_relay)
                _relay->onSessionActive(_peerId);
        }
        else if (state == wrtc::PeerSession::State::Ended) {
            if (_relay)
                _relay->onSessionEnded(_peerId);
            stopRelay();
            stopStreaming();
            stopRecording();
        }
    };

    _session->DataReceived += [this](rtc::message_variant msg) {
        if (auto* text = std::get_if<std::string>(&msg))
            LDebug("Data from ", _peerId, ": ", *text);
    };

    _session->media().KeyframeRequested += [this]() {
        LDebug("PLI from ", _peerId, ": keyframe requested");
        if (_config.mode == Config::Mode::Relay && _relay) {
            _relay->onViewerKeyframeRequested(_peerId);
            return;
        }
    };

    _session->media().BitrateEstimate += [this](unsigned int bps) {
        if (_config.mode == Config::Mode::Relay && _relay) {
            _relay->onViewerBitrateEstimate(_peerId, bps);
            return;
        }
        std::lock_guard lock(_mutex);
        LDebug("REMB from ", _peerId, ": ", bps / 1000, " kbps");
        if (_videoEncoder && _videoEncoder->ctx) {
            _videoEncoder->ctx->bit_rate = static_cast<int64_t>(bps);
            _videoEncoder->ctx->rc_max_rate = static_cast<int64_t>(bps);
        }
    };
}


MediaSession::~MediaSession()
{
    stopRelay();
    stopRecording();
    stopStreaming();
    _stream.close();
}


wrtc::PeerSession& MediaSession::session()
{
    return *_session;
}


const std::string& MediaSession::peerId() const
{
    return _peerId;
}


void MediaSession::onSignallingMessage(const json::Value& msg)
{
    if (_signaller)
        _signaller->onMessage(msg);
}


bool MediaSession::active() const
{
    return _session && _session->state() == wrtc::PeerSession::State::Active;
}


void MediaSession::relayVideo(av::VideoPacket& packet)
{
    if (_session && _session->media().hasVideo())
        _session->media().videoSender().process(packet);
}


void MediaSession::relayAudio(av::AudioPacket& packet)
{
    if (_session && _session->media().hasAudio())
        _session->media().audioSender().process(packet);
}


void MediaSession::requestRelayKeyframe()
{
    if (_session && _session->media().hasVideo())
        _session->media().requestKeyframe();
}


void MediaSession::requestRelayBitrate(unsigned int bps)
{
    if (_session && _session->media().hasVideo())
        _session->media().requestBitrate(bps);
}


av::VideoCodec MediaSession::makeVideoCodec(const Config& config)
{
    return wrtc::CodecNegotiator::resolveWebRtcVideoCodec(
        av::VideoCodec("H264",
                       config.videoCodec,
                       config.videoWidth,
                       config.videoHeight,
                       config.videoFps,
                       config.videoBitRate));
}


av::AudioCodec MediaSession::makeAudioCodec(const Config& config)
{
    return wrtc::CodecNegotiator::resolveWebRtcAudioCodec(
        av::AudioCodec("opus",
                       config.audioCodec,
                       config.audioChannels,
                       config.audioSampleRate,
                       config.audioBitRate,
                       "flt"));
}


void MediaSession::startStreaming()
{
    if (_config.mode != Config::Mode::Stream)
        return;
    if (!_session)
        return;
    if (_config.source.empty()) {
        LWarn("No media source configured");
        return;
    }

    if (!_capture) {
        _capture = std::make_shared<av::MediaCapture>();
        _capture->openFile(_config.source);
        _capture->setLoopInput(_config.loop);
        _capture->setLimitFramerate(true);
    }

    if (!_streamReady) {
        _stream.attachSource(_capture.get(), false, true);

        if (_session->media().hasVideo()) {
            _videoEncoder = std::make_shared<av::VideoPacketEncoder>();
            _capture->getEncoderVideoCodec(_videoEncoder->iparams);
            _videoEncoder->oparams = makeVideoCodec(_config);
            _videoSender = &_session->media().videoSender();
            _stream.attach(_videoEncoder, 1, true);
            _stream.attach(_videoSender, 5, false);
        }

        if (_session->media().hasAudio()) {
            _audioEncoder = std::make_shared<av::AudioPacketEncoder>();
            _capture->getEncoderAudioCodec(_audioEncoder->iparams);
            _audioEncoder->oparams = makeAudioCodec(_config);
            _audioSender = &_session->media().audioSender();
            _stream.attach(_audioEncoder, 2, true);
            _stream.attach(_audioSender, 6, false);
        }

        _streamReady = true;
    }

    _stream.start();
    _capture->start();
    LInfo("Streaming to ", _peerId);
}


void MediaSession::stopStreaming()
{
    if (_capture)
        _capture->stop();
    _stream.stop();
}


void MediaSession::startRecording()
{
    if (_config.mode != Config::Mode::Record || !_recorder || !_session)
        return;

    try {
        _recorder->start(_session->media());
    }
    catch (const std::exception& exc) {
        LError("MediaSession startRecording failed: ", exc.what());
        throw;
    }
}


void MediaSession::stopRecording()
{
    if (_recorder)
        _recorder->stop();
}


void MediaSession::startRelay()
{
    if (_config.mode != Config::Mode::Relay || _relayAttached || !_session)
        return;

    _session->media().videoReceiver().emitter +=
        packetSlot(this, &MediaSession::onRelayedVideo);
    _session->media().audioReceiver().emitter +=
        packetSlot(this, &MediaSession::onRelayedAudio);
    _relayAttached = true;
}


void MediaSession::stopRelay()
{
    if (!_relayAttached || !_session)
        return;

    _session->media().videoReceiver().emitter -= this;
    _session->media().audioReceiver().emitter -= this;
    _relayAttached = false;
}


void MediaSession::onRelayedVideo(av::VideoPacket& packet)
{
    if (_relay)
        _relay->relayVideo(_peerId, packet);
}


void MediaSession::onRelayedAudio(av::AudioPacket& packet)
{
    if (_relay)
        _relay->relayAudio(_peerId, packet);
}


void RelayController::registerSession(const std::shared_ptr<MediaSession>& session)
{
    std::lock_guard lock(_mutex);
    _sessions[session->peerId()] = session;
}


void RelayController::unregisterSession(const std::string& peerId)
{
    std::shared_ptr<MediaSession> source;
    unsigned int minBitrate = 0;
    {
        std::lock_guard lock(_mutex);
        _sessions.erase(peerId);
        _viewerBitrates.erase(peerId);
        if (_sourcePeerId == peerId) {
            source = electSourceLocked();
            minBitrate = minViewerBitrateLocked();
        }
        else {
            source = currentSourceLocked();
            minBitrate = minViewerBitrateLocked();
        }
    }

    if (source) {
        source->requestRelayKeyframe();
        if (minBitrate > 0)
            source->requestRelayBitrate(minBitrate);
    }
}


void RelayController::onSessionActive(const std::string& peerId)
{
    std::shared_ptr<MediaSession> source;
    bool requestKeyframe = false;
    {
        std::lock_guard lock(_mutex);
        source = currentSourceLocked();
        if (!source) {
            auto it = _sessions.find(peerId);
            if (it != _sessions.end())
                source = it->second.lock();
            if (source && source->active()) {
                _sourcePeerId = peerId;
                _viewerBitrates.erase(peerId);
                requestKeyframe = relayTargetsLocked(peerId).size() > 0;
                LInfo("Relay source set to ", peerId);
            }
        }
        else if (_sourcePeerId != peerId) {
            requestKeyframe = true;
            LInfo("Relay viewer joined: ", peerId, " source=", _sourcePeerId);
        }
    }

    if (requestKeyframe && source)
        source->requestRelayKeyframe();
}


void RelayController::onSessionEnded(const std::string& peerId)
{
    std::shared_ptr<MediaSession> source;
    unsigned int minBitrate = 0;
    {
        std::lock_guard lock(_mutex);
        _viewerBitrates.erase(peerId);
        if (_sourcePeerId == peerId) {
            _sourcePeerId.clear();
            source = electSourceLocked();
        }
        else {
            source = currentSourceLocked();
        }
        minBitrate = minViewerBitrateLocked();
    }

    if (source) {
        source->requestRelayKeyframe();
        if (minBitrate > 0)
            source->requestRelayBitrate(minBitrate);
    }
}


void RelayController::onViewerKeyframeRequested(const std::string& peerId)
{
    std::shared_ptr<MediaSession> source;
    {
        std::lock_guard lock(_mutex);
        if (_sourcePeerId.empty() || _sourcePeerId == peerId)
            return;
        source = currentSourceLocked();
    }

    if (source)
        source->requestRelayKeyframe();
}


void RelayController::onViewerBitrateEstimate(const std::string& peerId, unsigned int bps)
{
    std::shared_ptr<MediaSession> source;
    unsigned int minBitrate = 0;
    {
        std::lock_guard lock(_mutex);
        if (_sourcePeerId.empty() || _sourcePeerId == peerId)
            return;
        _viewerBitrates[peerId] = bps;
        source = currentSourceLocked();
        minBitrate = minViewerBitrateLocked();
    }

    if (source && minBitrate > 0)
        source->requestRelayBitrate(minBitrate);
}


void RelayController::relayVideo(const std::string& peerId, av::VideoPacket& packet)
{
    std::vector<std::shared_ptr<MediaSession>> targets;
    {
        std::lock_guard lock(_mutex);
        targets = relayTargetsLocked(peerId);
    }

    for (auto& target : targets)
        target->relayVideo(packet);
}


void RelayController::relayAudio(const std::string& peerId, av::AudioPacket& packet)
{
    std::vector<std::shared_ptr<MediaSession>> targets;
    {
        std::lock_guard lock(_mutex);
        targets = relayTargetsLocked(peerId);
    }

    for (auto& target : targets)
        target->relayAudio(packet);
}


void RelayController::clear()
{
    std::lock_guard lock(_mutex);
    _sessions.clear();
    _viewerBitrates.clear();
    _sourcePeerId.clear();
}


std::shared_ptr<MediaSession> RelayController::currentSourceLocked()
{
    if (_sourcePeerId.empty())
        return nullptr;

    auto it = _sessions.find(_sourcePeerId);
    if (it == _sessions.end()) {
        _sourcePeerId.clear();
        return nullptr;
    }

    auto session = it->second.lock();
    if (!session || !session->active()) {
        if (!session)
            _sessions.erase(it);
        _sourcePeerId.clear();
        return nullptr;
    }

    return session;
}


std::shared_ptr<MediaSession> RelayController::electSourceLocked()
{
    std::shared_ptr<MediaSession> candidate;
    std::string candidateId;

    for (auto it = _sessions.begin(); it != _sessions.end();) {
        auto session = it->second.lock();
        if (!session) {
            _viewerBitrates.erase(it->first);
            it = _sessions.erase(it);
            continue;
        }
        if (session->active() && (candidateId.empty() || it->first < candidateId)) {
            candidate = session;
            candidateId = it->first;
        }
        ++it;
    }

    _sourcePeerId = candidateId;
    if (!candidateId.empty()) {
        _viewerBitrates.erase(candidateId);
        LInfo("Relay source promoted to ", candidateId);
    }
    return candidate;
}


std::vector<std::shared_ptr<MediaSession>>
RelayController::relayTargetsLocked(const std::string& sourcePeerId)
{
    std::vector<std::shared_ptr<MediaSession>> targets;
    auto source = currentSourceLocked();
    if (!source || _sourcePeerId != sourcePeerId)
        return targets;

    targets.reserve(_sessions.size());
    for (auto it = _sessions.begin(); it != _sessions.end();) {
        auto session = it->second.lock();
        if (!session) {
            _viewerBitrates.erase(it->first);
            it = _sessions.erase(it);
            continue;
        }
        if (it->first != sourcePeerId && session->active())
            targets.push_back(std::move(session));
        ++it;
    }
    return targets;
}


unsigned int RelayController::minViewerBitrateLocked() const
{
    unsigned int minBitrate = 0;
    for (const auto& [peerId, bitrate] : _viewerBitrates) {
        if (peerId == _sourcePeerId)
            continue;
        if (bitrate == 0)
            continue;
        if (minBitrate == 0 || bitrate < minBitrate)
            minBitrate = bitrate;
    }
    return minBitrate;
}


} // namespace media_server
} // namespace icy
