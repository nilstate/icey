///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/peersession.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"

#include <optional>
#include <stdexcept>
#include <string_view>


namespace icy {
namespace wrtc {

namespace {

struct MediaSectionBounds
{
    size_t begin = std::string::npos;
    size_t end = std::string::npos;
};


std::optional<MediaSectionBounds> findMediaSection(const std::string& sdp,
                                                   std::string_view mediaType)
{
    std::string sectionHeader = "m=" + std::string(mediaType) + " ";
    size_t section = sdp.find(sectionHeader);
    if (section == std::string::npos)
        return std::nullopt;

    size_t sectionEnd = sdp.find("\r\nm=", section + 2);
    return MediaSectionBounds{section, sectionEnd == std::string::npos ? sdp.size() : sectionEnd};
}


std::optional<std::string> findSectionValue(const std::string& sdp,
                                            const MediaSectionBounds& section,
                                            std::string_view prefix)
{
    size_t searchPos = section.begin;
    while (true) {
        size_t valuePos = sdp.find(prefix, searchPos);
        if (valuePos == std::string::npos || valuePos >= section.end)
            break;

        size_t valueStart = valuePos + prefix.size();
        size_t valueEnd = sdp.find("\r\n", valueStart);
        if (valueEnd == std::string::npos)
            valueEnd = sdp.size();
        return sdp.substr(valueStart, valueEnd - valueStart);
    }

    return std::nullopt;
}


bool iequalsAscii(std::string_view a, std::string_view b)
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}


std::optional<CodecSpec> findSupportedCodecForMediaType(const std::string& sdp,
                                                        std::string_view mediaType,
                                                        CodecMediaType codecType)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;

    std::string_view sectionText(sdp.data() + section->begin, section->end - section->begin);
    return CodecNegotiator::detectCodec(sectionText, codecType);
}


std::optional<int> findPayloadTypeForCodec(const std::string& sdp,
                                           std::string_view mediaType,
                                           std::string_view codecName)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;

    size_t lineStart = section->begin;
    while (lineStart < section->end) {
        size_t lineEnd = sdp.find("\r\n", lineStart);
        if (lineEnd == std::string::npos || lineEnd > section->end)
            lineEnd = section->end;

        std::string_view line(sdp.data() + lineStart, lineEnd - lineStart);
        constexpr std::string_view prefix = "a=rtpmap:";
        if (line.substr(0, prefix.size()) == prefix) {
            auto space = line.find(' ', prefix.size());
            auto slash = line.find('/', space == std::string_view::npos ? prefix.size() : space + 1);
            if (space != std::string_view::npos && slash != std::string_view::npos) {
                auto ptText = line.substr(prefix.size(), space - prefix.size());
                auto name = line.substr(space + 1, slash - space - 1);
                if (iequalsAscii(name, codecName))
                    return std::stoi(std::string(ptText));
            }
        }

        lineStart = lineEnd + 2;
    }

    return std::nullopt;
}


std::optional<std::string> findMidForMediaType(const std::string& sdp,
                                               std::string_view mediaType)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;
    return findSectionValue(sdp, *section, "\r\na=mid:");
}


std::optional<rtc::Description::Direction> findDirectionForMediaType(
    const std::string& sdp,
    std::string_view mediaType)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;

    std::string_view sectionText(sdp.data() + section->begin, section->end - section->begin);
    if (sectionText.find("\r\na=sendonly") != std::string_view::npos)
        return rtc::Description::Direction::SendOnly;
    if (sectionText.find("\r\na=recvonly") != std::string_view::npos)
        return rtc::Description::Direction::RecvOnly;
    if (sectionText.find("\r\na=inactive") != std::string_view::npos)
        return rtc::Description::Direction::Inactive;
    return rtc::Description::Direction::SendRecv;
}


bool sends(rtc::Description::Direction direction)
{
    return direction == rtc::Description::Direction::SendRecv ||
           direction == rtc::Description::Direction::SendOnly;
}


bool receives(rtc::Description::Direction direction)
{
    return direction == rtc::Description::Direction::SendRecv ||
           direction == rtc::Description::Direction::RecvOnly;
}


bool receiveOnly(rtc::Description::Direction direction)
{
    return receives(direction) && !sends(direction);
}


rtc::Description::Direction negotiateAnswerDirection(
    rtc::Description::Direction localDirection,
    rtc::Description::Direction remoteOfferDirection)
{
    const bool localCanSend = sends(localDirection) && receives(remoteOfferDirection);
    const bool localCanReceive = receives(localDirection) && sends(remoteOfferDirection);
    if (localCanSend && localCanReceive)
        return rtc::Description::Direction::SendRecv;
    if (localCanSend)
        return rtc::Description::Direction::SendOnly;
    if (localCanReceive)
        return rtc::Description::Direction::RecvOnly;
    return rtc::Description::Direction::Inactive;
}


MediaBridge::Options offerScopedMediaOptions(const PeerSession::Config& config,
                                             const std::string& offerSdp)
{
    auto opts = config.mediaOpts;
    if (auto mid = findMidForMediaType(offerSdp, "audio")) {
        opts.audioMid = *mid;
        if (auto direction = findDirectionForMediaType(offerSdp, "audio"))
            opts.audioDirection = negotiateAnswerDirection(opts.audioDirection, *direction);
        if (opts.audioCodec.specified() || receiveOnly(opts.audioDirection)) {
            auto spec = CodecNegotiator::specFromAudioCodec(opts.audioCodec);
            auto payloadType = spec
                ? findPayloadTypeForCodec(offerSdp, "audio", spec->rtpName)
                : std::nullopt;
            if (payloadType) {
                opts.audioPayloadType = *payloadType;
            }
            else if (receiveOnly(opts.audioDirection)) {
                auto fallback = findSupportedCodecForMediaType(
                    offerSdp, "audio", CodecMediaType::Audio);
                if (fallback) {
                    opts.audioCodec = av::AudioCodec(
                        fallback->rtpName,
                        DEFAULT_AUDIO_CHANNELS,
                        static_cast<int>(fallback->clockRate));
                    opts.audioPayloadType = findPayloadTypeForCodec(
                        offerSdp, "audio", fallback->rtpName).value_or(fallback->payloadType);
                    LInfo("Negotiated receive-only audio codec: ", fallback->rtpName);
                }
                else {
                    LWarn("Remote offer does not contain a supported audio codec");
                    opts.audioCodec = {};
                    opts.audioDirection = rtc::Description::Direction::Inactive;
                }
            }
            else {
                LWarn("Remote offer does not contain the configured audio codec");
                opts.audioCodec = {};
                opts.audioDirection = rtc::Description::Direction::Inactive;
            }
        }
    }
    else {
        opts.audioCodec = {};
        opts.audioDirection = rtc::Description::Direction::Inactive;
    }
    if (auto mid = findMidForMediaType(offerSdp, "video")) {
        opts.videoMid = *mid;
        if (auto direction = findDirectionForMediaType(offerSdp, "video"))
            opts.videoDirection = negotiateAnswerDirection(opts.videoDirection, *direction);
        if (opts.videoCodec.specified() || receiveOnly(opts.videoDirection)) {
            auto spec = CodecNegotiator::specFromVideoCodec(opts.videoCodec);
            auto payloadType = spec
                ? findPayloadTypeForCodec(offerSdp, "video", spec->rtpName)
                : std::nullopt;
            if (payloadType) {
                opts.videoPayloadType = *payloadType;
            }
            else if (receiveOnly(opts.videoDirection)) {
                auto fallback = findSupportedCodecForMediaType(
                    offerSdp, "video", CodecMediaType::Video);
                if (fallback) {
                    opts.videoCodec = av::VideoCodec(
                        fallback->rtpName,
                        opts.videoCodec.width,
                        opts.videoCodec.height,
                        opts.videoCodec.fps,
                        opts.videoCodec.bitRate,
                        static_cast<int>(fallback->clockRate),
                        opts.videoCodec.pixelFmt);
                    opts.videoPayloadType = findPayloadTypeForCodec(
                        offerSdp, "video", fallback->rtpName).value_or(fallback->payloadType);
                    LInfo("Negotiated receive-only video codec: ", fallback->rtpName);
                }
                else {
                    LWarn("Remote offer does not contain a supported video codec");
                    opts.videoCodec = {};
                    opts.videoDirection = rtc::Description::Direction::Inactive;
                }
            }
            else {
                LWarn("Remote offer does not contain the configured video codec");
                opts.videoCodec = {};
                opts.videoDirection = rtc::Description::Direction::Inactive;
            }
        }
    }
    else {
        opts.videoCodec = {};
        opts.videoDirection = rtc::Description::Direction::Inactive;
    }
    return opts;
}

} // namespace


PeerSession::PeerSession(SignallingInterface& signaller, const Config& config)
    : _signaller(signaller)
    , _config(config)
    , _callbackSync([this]() { drainCallbacks(); })
{
    // PeerSession owns SDP exchange explicitly through SignallingInterface.
    // Auto-negotiation races against the init/accept/offer protocol and can
    // emit stray local offers on the incoming path.
    _config.rtcConfig.disableAutoNegotiation = true;

    _signaller.SdpReceived += slot(this, &PeerSession::onSdpReceived);
    _signaller.CandidateReceived += slot(this, &PeerSession::onCandidateReceived);
    _signaller.ControlReceived += slot(this, &PeerSession::onControlReceived);
}


PeerSession::~PeerSession()
{
    _callbackGuard->alive.store(false, std::memory_order_release);
    {
        std::lock_guard lock(_callbackMutex);
        _pendingCallbacks.clear();
    }
    _callbackSync.close();

    _signaller.SdpReceived.detach(this);
    _signaller.CandidateReceived.detach(this);
    _signaller.ControlReceived.detach(this);

    if (_state != State::Idle && _state != State::Ended) {
        try { hangup("destroyed"); } catch (...) {}
    }
}


void PeerSession::call(const std::string& peerId)
{
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Ended)
            _state = State::Idle;
        if (_state != State::Idle)
            throw std::logic_error(
                std::string("Cannot call: already in state ") + stateToString(_state));

        LInfo("Initiating call to ", peerId);
        _remotePeerId = peerId;
        remotePeerId = _remotePeerId;
        _state = State::OutgoingInit;
    }
    enqueueCallback([this]() { StateChanged.emit(State::OutgoingInit); });
    _signaller.sendControl(remotePeerId, "init");
}


void PeerSession::accept()
{
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state != State::IncomingInit)
            throw std::logic_error("Cannot accept: no incoming call");

        LInfo("Accepting call from ", _remotePeerId);
        _state = State::Negotiating;
        _remoteDescriptionSet = false;
        _pendingRemoteCandidates.clear();
        remotePeerId = _remotePeerId;
    }
    enqueueCallback([this]() { StateChanged.emit(State::Negotiating); });
    _signaller.sendControl(remotePeerId, "accept");
}


void PeerSession::reject(const std::string& reason)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state != State::IncomingInit)
            throw std::logic_error("Cannot reject: no incoming call");

        LInfo("Rejecting call from ", _remotePeerId, " reason=", reason);
        remotePeerId = _remotePeerId;
        beginEndCall("rejected", pc, dc);
    }
    enqueueCallback([this]() { StateChanged.emit(State::Ending); });
    _signaller.sendControl(remotePeerId, "reject", reason);
    if (dc)
        dc->close();
    if (pc)
        pc->close();
    finishEndCall();
    enqueueCallback([this]() { StateChanged.emit(State::Ended); });
    transitionEndedToIdle();
}


void PeerSession::hangup(const std::string& reason)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Idle || _state == State::Ended || _state == State::Ending)
            return;

        LInfo("Hanging up, reason=", reason);
        remotePeerId = _remotePeerId;
        beginEndCall(reason, pc, dc);
    }
    enqueueCallback([this]() { StateChanged.emit(State::Ending); });
    if (!remotePeerId.empty())
        _signaller.sendControl(remotePeerId, "hangup", reason);
    if (dc)
        dc->close();
    if (pc)
        pc->close();
    finishEndCall();
    enqueueCallback([this]() { StateChanged.emit(State::Ended); });
    transitionEndedToIdle();
}


void PeerSession::sendData(const std::string& message)
{
    std::lock_guard lock(_mutex);
    if (_dc && _dc->isOpen())
        _dc->send(message);
}


void PeerSession::sendData(const std::byte* data, size_t size)
{
    std::lock_guard lock(_mutex);
    if (_dc && _dc->isOpen())
        _dc->send(data, size);
}


PeerSession::State PeerSession::state() const
{
    std::lock_guard lock(_mutex);
    return _state;
}

std::string PeerSession::remotePeerId() const
{
    std::lock_guard lock(_mutex);
    return _remotePeerId;
}

MediaBridge& PeerSession::media() { return _media; }
const MediaBridge& PeerSession::media() const { return _media; }

std::shared_ptr<rtc::PeerConnection> PeerSession::peerConnection()
{
    std::lock_guard lock(_mutex);
    return _pc;
}

std::shared_ptr<rtc::DataChannel> PeerSession::dataChannel()
{
    std::lock_guard lock(_mutex);
    return _dc;
}


// ---------------------------------------------------------------------------
// Signalling callbacks
// ---------------------------------------------------------------------------

void PeerSession::onControlReceived(const std::string& peerId,
                                     const std::string& type,
                                     const std::string& reason)
{
    if (type == "init") {
        bool busy = false;
        {
            std::lock_guard lock(_mutex);
            if (_state == State::Ended)
                _state = State::Idle;
            if (_state != State::Idle) {
                busy = true;
            }
            else {
                _remotePeerId = peerId;
                _state = State::IncomingInit;
            }
        }
        if (busy) {
            _signaller.sendControl(peerId, "reject", "busy");
            return;
        }
        enqueueCallback([this]() { StateChanged.emit(State::IncomingInit); });
        enqueueCallback([this, peerId]() { IncomingCall.emit(peerId); });
    }
    else if (type == "accept") {
        bool shouldConnect = false;
        {
            std::lock_guard lock(_mutex);
            if (_state != State::OutgoingInit || peerId != _remotePeerId)
                return;

            LInfo("Remote accepted call");
            _state = State::Negotiating;
            shouldConnect = true;
        }
        if (shouldConnect) {
            auto pc = createPeerConnection(true);
            if (pc) {
                pc->setLocalDescription(rtc::Description::Type::Offer);
            }
        }
        enqueueCallback([this]() { StateChanged.emit(State::Negotiating); });
    }
    else if (type == "reject") {
        std::shared_ptr<rtc::PeerConnection> pc;
        std::shared_ptr<rtc::DataChannel> dc;
        {
            std::lock_guard lock(_mutex);
            if (_state != State::OutgoingInit || peerId != _remotePeerId)
                return;

            LInfo("Remote rejected call: ", reason);
            beginEndCall("rejected", pc, dc);
        }
        enqueueCallback([this]() { StateChanged.emit(State::Ending); });
        if (dc)
            dc->close();
        if (pc)
            pc->close();
        finishEndCall();
        enqueueCallback([this]() { StateChanged.emit(State::Ended); });
        transitionEndedToIdle();
    }
    else if (type == "hangup") {
        std::shared_ptr<rtc::PeerConnection> pc;
        std::shared_ptr<rtc::DataChannel> dc;
        {
            std::lock_guard lock(_mutex);
            if (peerId != _remotePeerId)
                return;

            LInfo("Remote hung up: ", reason);
            beginEndCall(reason, pc, dc);
        }
        enqueueCallback([this]() { StateChanged.emit(State::Ending); });
        if (dc)
            dc->close();
        if (pc)
            pc->close();
        finishEndCall();
        enqueueCallback([this]() { StateChanged.emit(State::Ended); });
        transitionEndedToIdle();
    }
}


void PeerSession::onSdpReceived(const std::string& peerId,
                                 const std::string& type,
                                 const std::string& sdp)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    bool createIncomingPc = false;
    std::optional<MediaBridge::Options> mediaOpts;
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId)
            return;
        if (_state != State::Negotiating && _state != State::Active)
            return;

        if (!_pc) {
            if (type != "offer" || _state != State::Negotiating)
                return;
            createIncomingPc = true;
            mediaOpts = offerScopedMediaOptions(_config, sdp);
        }
        else {
            pc = _pc;
        }
    }

    if (createIncomingPc) {
        pc = createPeerConnection(false, mediaOpts ? &*mediaOpts : nullptr);
        if (!pc)
            return;
    }

    std::vector<PendingCandidate> pendingCandidates;
    if (type == "offer") {
        LDebug("Processing remote offer");
        pc->setRemoteDescription(rtc::Description(sdp, type));
        pc->setLocalDescription(rtc::Description::Type::Answer);
    }
    else if (type == "answer") {
        LDebug("Processing remote answer");
        pc->setRemoteDescription(rtc::Description(sdp, type));
    }

    {
        std::lock_guard lock(_mutex);
        if (_pc != pc)
            return;
        _remoteDescriptionSet = true;
        pendingCandidates.swap(_pendingRemoteCandidates);
    }

    for (const auto& pending : pendingCandidates) {
        LDebug("Adding deferred remote ICE candidate");
        pc->addRemoteCandidate(rtc::Candidate(pending.candidate, pending.mid));
    }
}


void PeerSession::onCandidateReceived(const std::string& peerId,
                                       const std::string& candidate,
                                       const std::string& mid)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    bool defer = false;
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId)
            return;
        if (_state != State::Negotiating && _state != State::Active)
            return;
        pc = _pc;
        if (!_pc || !_remoteDescriptionSet) {
            _pendingRemoteCandidates.push_back({candidate, mid});
            defer = true;
        }
    }

    if (defer) {
        LDebug("Queueing remote ICE candidate until remote description is set");
        return;
    }

    LDebug("Adding remote ICE candidate");
    pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}


// ---------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------

std::shared_ptr<rtc::PeerConnection> PeerSession::createPeerConnection(
    bool createDataChannel,
    const MediaBridge::Options* mediaOpts)
{
    auto pc = std::make_shared<rtc::PeerConnection>(_config.rtcConfig);
    std::shared_ptr<rtc::DataChannel> dc;
    auto callbackGuard = _callbackGuard;
    setupPeerConnectionCallbacks(pc);

    if (createDataChannel && _config.enableDataChannel) {
        dc = pc->createDataChannel(_config.dataChannelLabel);
        dc->onOpen([callbackGuard]() {
            if (!callbackGuard->alive.load(std::memory_order_acquire))
                return;
            LDebug("Data channel open");
        });
        dc->onMessage([this, callbackGuard](rtc::message_variant msg) {
            if (!callbackGuard->alive.load(std::memory_order_acquire))
                return;
            enqueueCallback([this, msg = std::move(msg)]() mutable {
                DataReceived.emit(std::move(msg));
            });
        });
    }

    pc->onDataChannel([this, pc, callbackGuard](std::shared_ptr<rtc::DataChannel> dc) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        std::lock_guard lock(_mutex);
        if (_pc != pc || _dc)
            return;

        _dc = dc;
        _dc->onMessage([this, callbackGuard](rtc::message_variant msg) {
            if (!callbackGuard->alive.load(std::memory_order_acquire))
                return;
            enqueueCallback([this, msg = std::move(msg)]() mutable {
                DataReceived.emit(std::move(msg));
            });
        });
    });

    {
        std::lock_guard lock(_mutex);
        _pc = std::move(pc);
        _dc = std::move(dc);
        _remoteDescriptionSet = false;
        _pendingRemoteCandidates.clear();
        pc = _pc;
    }

    if (!_media.attached())
        _media.attach(pc, mediaOpts ? *mediaOpts : _config.mediaOpts);

    return pc;
}


void PeerSession::setupPeerConnectionCallbacks(const std::shared_ptr<rtc::PeerConnection>& pc)
{
    auto callbackGuard = _callbackGuard;

    pc->onLocalDescription([this, pc, callbackGuard](rtc::Description desc) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        LDebug("Local SDP ready: ", desc.typeString());
        std::string peerId;
        {
            std::lock_guard lock(_mutex);
            if (_pc != pc)
                return;
            peerId = _remotePeerId;
        }
        if (!peerId.empty())
            _signaller.sendSdp(peerId, desc.typeString(), std::string(desc));
    });

    pc->onLocalCandidate([this, pc, callbackGuard](rtc::Candidate candidate) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        LDebug("Local ICE candidate");
        std::string peerId;
        {
            std::lock_guard lock(_mutex);
            if (_pc != pc)
                return;
            peerId = _remotePeerId;
        }
        if (!peerId.empty())
            _signaller.sendCandidate(peerId, std::string(candidate), candidate.mid());
    });

    pc->onStateChange([this, pc, callbackGuard](rtc::PeerConnection::State rtcState) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        LDebug("PeerConnection state: ", static_cast<int>(rtcState));

        State newState = State::Idle;
        bool changed = false;
        std::shared_ptr<rtc::PeerConnection> endedPc;
        std::shared_ptr<rtc::DataChannel> dc;
        bool shouldFinish = false;

        {
            std::lock_guard lock(_mutex);
            if (_pc != pc)
                return;
            switch (rtcState) {
            case rtc::PeerConnection::State::Connected:
                if (_state == State::Negotiating) {
                    _state = State::Active;
                    newState = State::Active;
                    changed = true;
                }
                break;
            case rtc::PeerConnection::State::Failed:
                beginEndCall("connection failed", endedPc, dc);
                newState = State::Ending;
                changed = true;
                shouldFinish = true;
                break;
            case rtc::PeerConnection::State::Disconnected:
                beginEndCall("disconnected", endedPc, dc);
                newState = State::Ending;
                changed = true;
                shouldFinish = true;
                break;
            case rtc::PeerConnection::State::Closed:
                if (_state != State::Idle && _state != State::Ended && _state != State::Ending) {
                    beginEndCall("closed", endedPc, dc);
                    newState = State::Ending;
                    changed = true;
                    shouldFinish = true;
                }
                break;
            default:
                break;
            }
        }

        if (dc)
            dc->close();
        if (endedPc)
            endedPc->close();

        if (changed) {
            enqueueCallback([this, newState]() { StateChanged.emit(newState); });
            if (shouldFinish) {
                finishEndCall();
                enqueueCallback([this]() { StateChanged.emit(State::Ended); });
                transitionEndedToIdle();
            }
        }
    });
}


void PeerSession::beginEndCall(const std::string& reason,
                               std::shared_ptr<rtc::PeerConnection>& pc,
                               std::shared_ptr<rtc::DataChannel>& dc)
{
    LDebug("Ending call: ", reason);
    _state = State::Ending;
    _media.detach();

    dc = std::move(_dc);
    pc = std::move(_pc);
    _remoteDescriptionSet = false;
    _pendingRemoteCandidates.clear();

    _remotePeerId.clear();
}


void PeerSession::finishEndCall()
{
    std::lock_guard lock(_mutex);
    if (_state == State::Ending)
        _state = State::Ended;
}


void PeerSession::transitionEndedToIdle()
{
    bool changed = false;
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Ended && !_pc && !_dc && _remotePeerId.empty()) {
            _state = State::Idle;
            changed = true;
        }
    }

    if (changed)
        enqueueCallback([this]() { StateChanged.emit(State::Idle); });
}


void PeerSession::enqueueCallback(std::function<void()> callback)
{
    if (!_callbackGuard->alive.load(std::memory_order_acquire))
        return;

    {
        std::lock_guard lock(_callbackMutex);
        _pendingCallbacks.emplace_back(std::move(callback));
    }
    _callbackSync.post();
}


void PeerSession::drainCallbacks()
{
    std::deque<std::function<void()>> pending;
    {
        std::lock_guard lock(_callbackMutex);
        pending.swap(_pendingCallbacks);
    }

    if (!_callbackGuard->alive.load(std::memory_order_acquire))
        return;

    for (auto& callback : pending) {
        if (!_callbackGuard->alive.load(std::memory_order_acquire))
            return;
        callback();
    }
}


const char* stateToString(PeerSession::State state)
{
    switch (state) {
    case PeerSession::State::Idle:         return "idle";
    case PeerSession::State::OutgoingInit: return "outgoing-init";
    case PeerSession::State::IncomingInit: return "incoming-init";
    case PeerSession::State::Negotiating:  return "negotiating";
    case PeerSession::State::Active:       return "active";
    case PeerSession::State::Ending:       return "ending";
    case PeerSession::State::Ended:        return "ended";
    }
    return "unknown";
}


} // namespace wrtc
} // namespace icy
