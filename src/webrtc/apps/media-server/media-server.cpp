///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Media Server
//
// Single binary: WebRTC media streaming + Symple signalling + TURN relay
// + web UI. No Node.js, no third-party services. One binary, two ports.
//
// The media server registers as a virtual peer in the Symple network.
// Browsers discover it via presence, click Call, and receive video.
//
// Pipeline (stream mode):
//   MediaCapture (file) → VideoEncoder → WebRtcTrackSender → [browser]
//
// Usage:
//   media-server --source video.mp4 --web-root web/dist
//
/// @addtogroup webrtc
/// @{


#include "scy/application.h"
#include "scy/av/mediacapture.h"
#include "scy/json/json.h"
#include "scy/logger.h"
#include "scy/packetstream.h"
#include "scy/symple/server.h"
#include "scy/webrtc/peersession.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>


using namespace scy;


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
        }
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
                 const Config& config,
                 std::shared_ptr<av::MediaCapture> capture)
        : _peerId(peerId)
        , _stream("media-" + peerId)
        , _config(config)
        , _capture(std::move(capture))
    {
        wrtc::PeerSession::Config pc;
        pc.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");

        if (config.mode == Config::Mode::Stream) {
            pc.mediaOpts.videoCodec = av::VideoCodec(
                "H264", config.videoCodec,
                config.videoWidth, config.videoHeight, config.videoFps);
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
                _stream.stop();
        };

        _session->DataReceived += [this](rtc::message_variant msg) {
            if (auto* text = std::get_if<std::string>(&msg))
                LDebug("Data from ", _peerId, ": ", *text);
        };
    }

    ~MediaSession()
    {
        _stream.stop();
    }

    wrtc::PeerSession& session() { return *_session; }

private:
    void startStreaming()
    {
        if (_config.mode != Config::Mode::Stream)
            return;
        if (!_session || !_session->media().hasVideo())
            return;
        if (!_capture) {
            LWarn("No media source configured");
            return;
        }

        _stream.attachSource(_capture.get(), false, true);
        _stream.attach(&_session->media().videoSender(), 5, false);
        _stream.start();

        LInfo("Streaming to ", _peerId);
    }

    std::string _peerId;
    PacketStream _stream;
    const Config& _config;
    std::shared_ptr<av::MediaCapture> _capture;
    std::unique_ptr<wrtc::PeerSession> _session;
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
        // Media capture (shared across sessions)
        if (_config.mode == Config::Mode::Stream && !_config.source.empty()) {
            _capture = std::make_shared<av::MediaCapture>();
            _capture->openFile(_config.source);
            _capture->setLoopInput(_config.loop);
            _capture->setLimitFramerate(true);
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
        _signaller->ControlReceived += [this](const std::string& peerId,
                                               const std::string& type,
                                               const std::string& reason) {
            if (type == "init") {
                createSession(peerId);
            }
        };

        smpl::Peer vpeer;
        vpeer["id"] = _serverPeerId;
        vpeer["user"] = "media-server";
        vpeer["name"] = "Media Server";
        vpeer["type"] = "media-server";
        vpeer["online"] = true;

        _symple.addVirtualPeer(vpeer, {"public"},
            [this](const json::Value& msg) {
                _signaller->onMessage(msg);
            });

        // Start capture after everything is wired
        if (_capture)
            _capture->start();

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
        if (_capture) {
            _capture->stop();
            _capture.reset();
        }
        _symple.shutdown();
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
            peerId, *_signaller, _config, _capture);
        _sessions[peerId] = std::move(session);
    }

    Config _config;
    smpl::Server _symple;
    std::unique_ptr<ServerSignaller> _signaller;
    std::string _serverPeerId;
    std::string _serverAddress;
    std::shared_ptr<av::MediaCapture> _capture;
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
        else if (arg == "--mode" && !val.empty()) {
            config.mode = Config::parseMode(val); ++i;
        }
        else if (arg == "--source" && !val.empty()) {
            config.source = val; ++i;
        }
        else if (arg == "--web-root" && !val.empty()) {
            config.webRoot = val; ++i;
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
