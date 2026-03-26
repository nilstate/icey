#include "app.h"

#include "config.h"
#include "httpfactory.h"
#include "media.h"
#include "turnserver.h"

#include "icy/filesystem.h"
#include "icy/logger.h"

#include <fstream>
#include <iostream>


namespace icy {
namespace media_server {


MediaServerApp::MediaServerApp(const Config& config)
    : _config(config)
    , _relay(std::make_unique<RelayController>())
{
}


MediaServerApp::~MediaServerApp() = default;


void MediaServerApp::start()
{
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
    else if (_config.mode == Config::Mode::Record) {
        fs::mkdirr(_config.recordDir);
    }

    if (_config.enableTurn) {
        _turn = std::make_unique<EmbeddedTurn>(_config);
        _turn->start();
        std::cout << "TURN server listening on "
                  << _config.host << ":" << _config.turnPort << '\n';
    }

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
        _relay->unregisterSession(peer.id());
        _sessions.erase(peer.id());
    };

    _symple.start(symOpts,
                  std::make_unique<HttpFactory>(
                      _config.webRoot,
                      HttpFactory::RuntimeConfig{
                          _config.enableTurn,
                          _config.turnPort,
                          _config.turnExternalIP,
                          _config.host
                      }));

    _serverPeerId = "media-server";
    _serverAddress = "media-server|" + _serverPeerId;

    smpl::Peer vpeer;
    vpeer.setID(_serverPeerId);
    vpeer.setUser("media-server");
    vpeer.setName("Media Server");
    vpeer.setType("media-server");
    vpeer["online"] = true;
    switch (_config.mode) {
    case Config::Mode::Stream:
        vpeer["mode"] = "stream";
        vpeer["capabilities"] = json::Value::array({"view"});
        break;
    case Config::Mode::Record:
        vpeer["mode"] = "record";
        vpeer["capabilities"] = json::Value::array({"publish"});
        break;
    case Config::Mode::Relay:
        vpeer["mode"] = "relay";
        vpeer["capabilities"] = json::Value::array({"publish", "view"});
        break;
    }

    _symple.addVirtualPeer(vpeer, {"public"},
        [this](const json::Value& msg) {
            auto it = msg.find("subtype");
            if (it == msg.end())
                return;

            smpl::Address from(msg.value("from", ""));
            if (!from.valid() || from.id.empty() || from.user.empty())
                return;
            auto peerAddress = from.toString();

            std::shared_ptr<MediaSession> session;
            const auto& subtype = it->get_ref<const std::string&>();
            if (subtype == "call:init") {
                session = ensureSession(peerAddress);
            }
            else {
                auto found = _sessions.find(peerAddress);
                if (found != _sessions.end())
                    session = found->second;
            }
            if (session)
                session->onSignallingMessage(msg);
        });

    std::cout << "Media server listening on "
              << _config.host << ":" << _config.port << '\n';
    std::cout << "Web UI: http://localhost:" << _config.port << "/\n";
    if (_config.mode == Config::Mode::Stream && !_config.source.empty())
        std::cout << "Source: " << _config.source << '\n';
    if (_config.mode == Config::Mode::Record)
        std::cout << "Recordings: " << _config.recordDir << '\n';
    if (_config.mode == Config::Mode::Relay)
        std::cout << "Relay mode: first active caller becomes the live source; "
                     "other callers receive that feed\n";
}


void MediaServerApp::shutdown()
{
    _sessions.clear();
    _relay->clear();
    _symple.removeVirtualPeer(_serverPeerId);
    _symple.shutdown();
    if (_turn)
        _turn->stop();
}


std::shared_ptr<MediaSession> MediaServerApp::ensureSession(const std::string& peerAddress)
{
    auto it = _sessions.find(peerAddress);
    if (it != _sessions.end())
        return it->second;

    LInfo("Creating session for ", peerAddress);
    auto session = std::make_shared<MediaSession>(
        peerAddress, _symple, _serverAddress, _config, _relay.get());
    _relay->registerSession(session);
    _sessions[peerAddress] = session;
    return session;
}


} // namespace media_server
} // namespace icy
