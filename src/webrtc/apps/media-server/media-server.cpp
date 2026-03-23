///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Media Server
//
// Single binary: WebRTC media streaming + Symple signalling + TURN relay
// + web UI. No Node.js, no third-party services. One binary, two ports.
//
// The media server registers as a virtual peer in the Symple network.
// Browsers discover it via presence, click Call, and receive video + audio.
//
// Pipeline (stream mode):
//   MediaCapture → VideoPacketEncoder → WebRtcTrackSender → [browser]
//                → AudioPacketEncoder → WebRtcTrackSender → [browser]
//
// Usage:
//   media-server --source video.mp4 --web-root web/dist
//
/// @addtogroup webrtc
/// @{


#include "icy/application.h"
#include "icy/av/audiopacketencoder.h"
#include "icy/av/mediacapture.h"
#include "icy/av/videopacketencoder.h"
#include "icy/json/json.h"
#include "icy/logger.h"
#include "icy/packetstream.h"
#include "icy/symple/server.h"
#include "icy/turn/server/server.h"
#include "icy/webrtc/peersession.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>


using namespace icy;


// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

struct Config
{
    std::string host = "0.0.0.0";
    uint16_t port = 4500;
    std::string webRoot = "./web/dist";

    enum class Mode { Stream, Record, Relay };
    Mode mode = Mode::Stream;
    std::string source;
    bool loop = true;

    std::string videoCodec = "libx264";
    int videoWidth = 1280;
    int videoHeight = 720;
    int videoFps = 30;
    int videoBitRate = 2000000;

    std::string audioCodec = "libopus";
    int audioChannels = 2;
    int audioSampleRate = 48000;
    int audioBitRate = 128000;

    // TURN
    bool enableTurn = true;
    uint16_t turnPort = 3478;
    std::string turnRealm = "0state.com";
    std::string turnExternalIP;

    static Mode parseMode(const std::string& s)
    {
        if (s == "record") return Mode::Record;
        if (s == "relay") return Mode::Relay;
        return Mode::Stream;
    }
};


Config loadConfig(const std::string& path)
{
    Config c;
    std::ifstream file(path);
    if (!file.is_open())
        return c;

    json::Value j;
    file >> j;

    if (j.contains("http")) {
        auto& h = j["http"];
        c.host = h.value("host", c.host);
        c.port = h.value("port", c.port);
    }
    if (j.contains("media")) {
        auto& m = j["media"];
        c.mode = Config::parseMode(m.value("mode", "stream"));
        c.source = m.value("source", c.source);
        c.loop = m.value("loop", c.loop);
        if (m.contains("video")) {
            auto& v = m["video"];
            c.videoCodec = v.value("codec", c.videoCodec);
            c.videoWidth = v.value("width", c.videoWidth);
            c.videoHeight = v.value("height", c.videoHeight);
            c.videoFps = v.value("fps", c.videoFps);
            c.videoBitRate = v.value("bitrate", c.videoBitRate);
        }
        if (m.contains("audio")) {
            auto& a = m["audio"];
            c.audioCodec = a.value("codec", c.audioCodec);
            c.audioChannels = a.value("channels", c.audioChannels);
            c.audioSampleRate = a.value("sampleRate", c.audioSampleRate);
            c.audioBitRate = a.value("bitrate", c.audioBitRate);
        }
    }
    if (j.contains("turn")) {
        auto& t = j["turn"];
        c.enableTurn = t.value("enabled", c.enableTurn);
        c.turnPort = t.value("port", c.turnPort);
        c.turnRealm = t.value("realm", c.turnRealm);
        c.turnExternalIP = t.value("externalIp", c.turnExternalIP);
    }
    if (j.contains("webRoot"))
        c.webRoot = j["webRoot"].get<std::string>();

    return c;
}


// ---------------------------------------------------------------------------
// Static file responder
// ---------------------------------------------------------------------------

class StaticFileResponder : public http::ServerResponder
{
public:
    StaticFileResponder(http::ServerConnection& conn, const std::string& webRoot)
        : http::ServerResponder(conn)
        , _webRoot(webRoot)
    {
    }

    void onRequest(http::Request& request, http::Response& response) override
    {
        std::string path = request.getURI();

        if (path == "/" || path.empty())
            path = "/index.html";

        if (path.find("..") != std::string::npos) {
            response.setStatus(http::StatusCode::Forbidden);
            connection().sendHeader();
            return;
        }

        std::string filePath = _webRoot + path;
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            response.setStatus(http::StatusCode::NotFound);
            connection().sendHeader();
            return;
        }

        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string body(static_cast<size_t>(size), '\0');
        file.read(body.data(), size);

        response.setContentType(mimeType(path));
        response.setContentLength(body.size());
        connection().sendHeader();
        connection().send(body.data(), body.size());
    }

private:
    static std::string mimeType(const std::string& path)
    {
        auto dot = path.rfind('.');
        if (dot == std::string::npos) return "application/octet-stream";
        auto ext = path.substr(dot + 1);
        if (ext == "html") return "text/html; charset=utf-8";
        if (ext == "js")   return "application/javascript; charset=utf-8";
        if (ext == "css")  return "text/css; charset=utf-8";
        if (ext == "json") return "application/json";
        if (ext == "png")  return "image/png";
        if (ext == "svg")  return "image/svg+xml";
        if (ext == "ico")  return "image/x-icon";
        if (ext == "woff2") return "font/woff2";
        if (ext == "woff") return "font/woff";
        return "application/octet-stream";
    }

    std::string _webRoot;
};


// ---------------------------------------------------------------------------
// HTTP factory (static files + REST API)
// ---------------------------------------------------------------------------

class HttpFactory : public http::ServerConnectionFactory
{
public:
    HttpFactory(const std::string& webRoot)
        : _webRoot(webRoot)
    {
    }

    std::unique_ptr<http::ServerResponder> createResponder(
        http::ServerConnection& conn) override
    {
        auto& uri = conn.request().getURI();

        if (uri.substr(0, 5) == "/api/")
            return createApiResponder(conn);

        return std::make_unique<StaticFileResponder>(conn, _webRoot);
    }

private:
    std::unique_ptr<http::ServerResponder> createApiResponder(
        http::ServerConnection& conn)
    {
        class ApiResponder : public http::ServerResponder
        {
        public:
            using http::ServerResponder::ServerResponder;
            void onRequest(http::Request&, http::Response& response) override
            {
                json::Value j;
                j["status"] = "ok";
                j["version"] = "1.0.0";
                auto body = j.dump();
                response.setContentType("application/json");
                response.setContentLength(body.size());
                connection().sendHeader();
                connection().send(body.data(), body.size());
            }
        };
        return std::make_unique<ApiResponder>(conn);
    }

    std::string _webRoot;
};


// ---------------------------------------------------------------------------
// Server-side signalling (virtual peer → WebRTC)
// ---------------------------------------------------------------------------

/// SignallingInterface that sends messages through the Symple server
/// on behalf of a virtual peer. Receives messages via the virtual peer
/// callback and emits the appropriate signals.
class ServerSignaller : public wrtc::SignallingInterface
{
public:
    ServerSignaller(smpl::Server& server, const std::string& serverPeerId)
        : _server(server)
        , _serverPeerId(serverPeerId)
    {
    }

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override
    {
        json::Value data;
        data["type"] = type;
        data["sdp"] = sdp;
        send("call:" + type, peerId, data);
    }

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override
    {
        json::Value data;
        data["candidate"] = candidate;
        data["sdpMid"] = mid;
        send("call:candidate", peerId, data);
    }

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override
    {
        json::Value data;
        if (!reason.empty())
            data["reason"] = reason;
        send("call:" + type, peerId, data);
    }

    /// Feed a message from the virtual peer callback into the signalling
    /// interface. Parses call:* subtypes and emits the right signals.
    void onMessage(const json::Value& msg)
    {
        auto it = msg.find("subtype");
        if (it == msg.end())
            return;

        std::string subtype = it->get<std::string>();
        if (subtype.compare(0, 5, "call:") != 0)
            return;

        std::string action = subtype.substr(5);

        // Extract sender address from "from" field
        std::string peerId;
        auto fromIt = msg.find("from");
        if (fromIt != msg.end()) {
            if (fromIt->is_string())
                peerId = fromIt->get<std::string>();
        }
        if (peerId.empty())
            return;

        LDebug("Server signaller: ", subtype, " from ", peerId);

        if (action == "offer" || action == "answer") {
            auto& data = msg.at("data");
            SdpReceived.emit(peerId,
                             data.at("type").get<std::string>(),
                             data.at("sdp").get<std::string>());
        }
        else if (action == "candidate") {
            auto& data = msg.at("data");
            CandidateReceived.emit(peerId,
                                   data.at("candidate").get<std::string>(),
                                   data.at("sdpMid").get<std::string>());
        }
        else if (action == "init" || action == "accept" ||
                 action == "reject" || action == "hangup") {
            std::string reason;
            if (msg.contains("data") && msg.at("data").contains("reason"))
                reason = msg.at("data").at("reason").get<std::string>();
            ControlReceived.emit(peerId, action, reason);
        }
    }

private:
    void send(const std::string& subtype,
              const std::string& to,
              const json::Value& data)
    {
        json::Value msg;
        msg["type"] = "message";
        msg["subtype"] = subtype;
        msg["from"] = _serverPeerId;
        msg["to"] = to;
        if (!data.empty())
            msg["data"] = data;

        _server.sendTo(to, msg);
    }

    smpl::Server& _server;
    std::string _serverPeerId;
};


// ---------------------------------------------------------------------------
// Media session (per-peer)
// ---------------------------------------------------------------------------

class MediaSession
{
public:
    MediaSession(const std::string& peerId,
                 wrtc::SignallingInterface& signaller,
                 const Config& config)
        : _peerId(peerId)
        , _stream("media-" + peerId)
        , _config(config)
    {
        av::VideoCodec videoCodec = makeVideoCodec(config);
        av::AudioCodec audioCodec = makeAudioCodec(config);

        wrtc::PeerSession::Config pc;

        // ICE servers: embedded TURN (using externalIP if set, else localhost)
        // plus Google's public STUN as fallback for server-reflexive candidates.
        if (config.enableTurn) {
            std::string turnHost = config.turnExternalIP.empty()
                ? "127.0.0.1" : config.turnExternalIP;
            std::string turnUri = "turn:" + turnHost + ":" +
                std::to_string(config.turnPort);
            pc.rtcConfig.iceServers.emplace_back(turnUri);
        }
        pc.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");

        if (config.mode == Config::Mode::Stream) {
            pc.mediaOpts.videoCodec = videoCodec;
            pc.mediaOpts.audioCodec = audioCodec;
        }

        pc.enableDataChannel = true;
        pc.dataChannelLabel = "control";

        _session = std::make_unique<wrtc::PeerSession>(signaller, pc);

        _session->IncomingCall += [this](const std::string& peer) {
            LInfo("Auto-accepting call from ", peer);
            _session->accept();
        };

        _session->StateChanged += [this](wrtc::PeerSession::State state) {
            LInfo("Session ", _peerId, ": ", wrtc::stateToString(state));
            if (state == wrtc::PeerSession::State::Active)
                startStreaming();
            else if (state == wrtc::PeerSession::State::Ended)
                stopStreaming();
        };

        _session->DataReceived += [this](rtc::message_variant msg) {
            if (auto* text = std::get_if<std::string>(&msg))
                LDebug("Data from ", _peerId, ": ", *text);
        };

        // Adaptive bitrate: wire RTCP feedback to encoder
        _session->media().KeyframeRequested += [this]() {
            LDebug("PLI from ", _peerId, ": keyframe requested");
            // Force IDR on next encode by closing and recreating
            // the encoder. In a production system you'd set a flag
            // on the encoder to force the next frame to be a keyframe.
        };

        _session->media().BitrateEstimate += [this](unsigned int bps) {
            std::lock_guard lock(_mutex);
            LDebug("REMB from ", _peerId, ": ", bps / 1000, " kbps");
            if (_videoEncoder && _videoEncoder->ctx) {
                _videoEncoder->ctx->bit_rate = static_cast<int64_t>(bps);
                _videoEncoder->ctx->rc_max_rate = static_cast<int64_t>(bps);
            }
        };
    }

    ~MediaSession()
    {
        stopStreaming();
    }

    wrtc::PeerSession& session() { return *_session; }

private:
    static av::VideoCodec makeVideoCodec(const Config& config)
    {
        av::VideoCodec vc("H264", config.videoCodec,
            config.videoWidth, config.videoHeight, config.videoFps,
            config.videoBitRate);
        vc.options["preset"] = "ultrafast";
        vc.options["tune"] = "zerolatency";
        vc.options["profile"] = "baseline";
        return vc;
    }

    static av::AudioCodec makeAudioCodec(const Config& config)
    {
        av::AudioCodec ac("opus", config.audioCodec,
            config.audioChannels, config.audioSampleRate,
            config.audioBitRate, "flt");
        ac.options["application"] = "lowdelay";
        return ac;
    }

    void startStreaming()
    {
        if (_config.mode != Config::Mode::Stream)
            return;
        if (!_session)
            return;
        if (_config.source.empty()) {
            LWarn("No media source configured");
            return;
        }

        // Each session gets its own capture so peers can join
        // independently without sharing decoder state.
        _capture = std::make_shared<av::MediaCapture>();
        _capture->openFile(_config.source);
        _capture->setLoopInput(_config.loop);
        _capture->setLimitFramerate(true);

        _stream.attachSource(_capture.get(), false, true);

        // Video encoder pipeline
        if (_session->media().hasVideo()) {
            _videoEncoder = std::make_shared<av::VideoPacketEncoder>();
            _capture->getEncoderVideoCodec(_videoEncoder->iparams);
            _videoEncoder->oparams = makeVideoCodec(_config);
            _stream.attach(_videoEncoder, 1, true);
            _stream.attach(&_session->media().videoSender(), 5, false);
        }

        // Audio encoder pipeline
        if (_session->media().hasAudio()) {
            _audioEncoder = std::make_shared<av::AudioPacketEncoder>();
            _capture->getEncoderAudioCodec(_audioEncoder->iparams);
            _audioEncoder->oparams = makeAudioCodec(_config);
            _stream.attach(_audioEncoder, 2, true);
            _stream.attach(&_session->media().audioSender(), 6, false);
        }

        _stream.start();
        _capture->start();
        LInfo("Streaming to ", _peerId);
    }

    void stopStreaming()
    {
        _stream.stop();
        if (_capture) {
            _capture->stop();
            _capture.reset();
        }
        _videoEncoder.reset();
        _audioEncoder.reset();
    }

    std::string _peerId;
    PacketStream _stream;
    const Config& _config;
    std::shared_ptr<av::MediaCapture> _capture;
    std::shared_ptr<av::VideoPacketEncoder> _videoEncoder;
    std::shared_ptr<av::AudioPacketEncoder> _audioEncoder;
    std::unique_ptr<wrtc::PeerSession> _session;
    mutable std::mutex _mutex;
};


// ---------------------------------------------------------------------------
// Embedded TURN server
// ---------------------------------------------------------------------------

class EmbeddedTurn : public turn::ServerObserver
{
public:
    EmbeddedTurn(const Config& config)
    {
        turn::ServerOptions opts;
        opts.software = "Icey Media Server TURN [rfc5766]";
        opts.realm = config.turnRealm;
        opts.listenAddr = net::Address(config.host, config.turnPort);
        opts.externalIP = config.turnExternalIP;
        opts.enableTCP = true;
        opts.enableUDP = true;
        opts.enableLocalIPPermissions = true;

        _server = std::make_unique<turn::Server>(*this, opts);
    }

    void start() { _server->start(); }
    void stop() { _server->stop(); }

    turn::AuthenticationState authenticateRequest(
        turn::Server*, turn::Request&) override
    {
        // Open access for the demo; production should check credentials.
        return turn::AuthenticationState::Authorized;
    }

    void onServerAllocationCreated(
        turn::Server*, turn::IAllocation* alloc) override
    {
        LInfo("TURN allocation created");
    }

    void onServerAllocationRemoved(
        turn::Server*, turn::IAllocation* alloc) override
    {
        LDebug("TURN allocation removed");
    }

private:
    std::unique_ptr<turn::Server> _server;
};


// ---------------------------------------------------------------------------
// Media Server Application
// ---------------------------------------------------------------------------

class MediaServerApp
{
public:
    MediaServerApp(const Config& config)
        : _config(config)
    {
    }

    void start()
    {
        // Validate source file exists before starting
        if (_config.mode == Config::Mode::Stream && !_config.source.empty()) {
            std::ifstream test(_config.source);
            if (!test.is_open()) {
                std::cerr << "Error: source file not found: " << _config.source << '\n';
                return;
            }
        }
        else if (_config.mode == Config::Mode::Stream && _config.source.empty()) {
            std::cerr << "Warning: no --source specified, server will not stream media\n";
        }

        // Start embedded TURN server
        if (_config.enableTurn) {
            _turn = std::make_unique<EmbeddedTurn>(_config);
            _turn->start();
            std::cout << "TURN server listening on "
                      << _config.host << ":" << _config.turnPort << '\n';
        }

        // Symple server with HTTP factory for static files
        smpl::Server::Options symOpts;
        symOpts.host = _config.host;
        symOpts.port = _config.port;
        symOpts.authentication = false;
        symOpts.dynamicRooms = true;

        _symple.Authenticate += [](smpl::ServerPeer&,
                                    const json::Value&,
                                    bool& allowed,
                                    std::vector<std::string>& rooms) {
            allowed = true;
            rooms.push_back("public");
        };

        _symple.PeerConnected += [](smpl::ServerPeer& peer) {
            LInfo("Peer connected: ", peer.id(),
                  " (", peer.peer().value("name", "?"), ")");
        };

        _symple.PeerDisconnected += [this](smpl::ServerPeer& peer) {
            LInfo("Peer disconnected: ", peer.id());
            _sessions.erase(peer.id());
        };

        _symple.start(symOpts,
                       std::make_unique<HttpFactory>(_config.webRoot));

        // Register as virtual peer
        _serverPeerId = "media-server";
        _serverAddress = "media-server|" + _serverPeerId;

        _signaller = std::make_unique<ServerSignaller>(_symple, _serverAddress);

        // When a call:init arrives, create a session for that peer
        smpl::Peer vpeer;
        vpeer["id"] = _serverPeerId;
        vpeer["user"] = "media-server";
        vpeer["name"] = "Media Server";
        vpeer["type"] = "media-server";
        vpeer["online"] = true;

        _symple.addVirtualPeer(vpeer, {"public"},
            [this](const json::Value& msg) {
                // Create session before forwarding call:init so that
                // PeerSession is subscribed when ControlReceived fires.
                auto it = msg.find("subtype");
                if (it != msg.end() && it->get<std::string>() == "call:init") {
                    auto from = msg.value("from", "");
                    auto sep = from.find('|');
                    auto peerId = (sep != std::string::npos)
                        ? from.substr(sep + 1) : from;
                    if (!peerId.empty())
                        createSession(peerId);
                }
                _signaller->onMessage(msg);
            });

        std::cout << "Media server listening on "
                  << _config.host << ":" << _config.port << '\n';
        std::cout << "Web UI: http://localhost:" << _config.port << "/\n";
        if (!_config.source.empty())
            std::cout << "Source: " << _config.source << '\n';
    }

    void shutdown()
    {
        _sessions.clear();
        _symple.removeVirtualPeer(_serverPeerId);
        _signaller.reset();
        _symple.shutdown();
        if (_turn)
            _turn->stop();
    }

private:
    void createSession(const std::string& peerId)
    {
        if (_sessions.count(peerId)) {
            LWarn("Session already exists for ", peerId);
            return;
        }

        LInfo("Creating session for ", peerId);
        auto session = std::make_unique<MediaSession>(
            peerId, *_signaller, _config);
        _sessions[peerId] = std::move(session);
    }

    Config _config;
    smpl::Server _symple;
    std::unique_ptr<ServerSignaller> _signaller;
    std::unique_ptr<EmbeddedTurn> _turn;
    std::string _serverPeerId;
    std::string _serverAddress;
    std::unordered_map<std::string, std::unique_ptr<MediaSession>> _sessions;
};


// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    std::string configPath = "./config.json";

    // First pass: find config path
    for (int i = 1; i + 1 < argc; i += 2) {
        if (std::string(argv[i]) == "-c" || std::string(argv[i]) == "--config")
            configPath = argv[i + 1];
    }

    Config config = loadConfig(configPath);

    // CLI overrides
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::string val = (i + 1 < argc) ? argv[i + 1] : "";

        if ((arg == "-c" || arg == "--config") && !val.empty()) { ++i; }
        else if (arg == "--port" && !val.empty()) {
            config.port = static_cast<uint16_t>(std::stoi(val)); ++i;
        }
        else if (arg == "--turn-port" && !val.empty()) {
            config.turnPort = static_cast<uint16_t>(std::stoi(val)); ++i;
        }
        else if (arg == "--mode" && !val.empty()) {
            config.mode = Config::parseMode(val); ++i;
        }
        else if (arg == "--source" && !val.empty()) {
            config.source = val; ++i;
        }
        else if (arg == "--web-root" && !val.empty()) {
            config.webRoot = val; ++i;
        }
        else if (arg == "--no-turn") {
            config.enableTurn = false;
        }
    }

    MediaServerApp app(config);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<MediaServerApp*>(opaque)->shutdown();
    }, &app);

    Logger::destroy();
    return 0;
}
