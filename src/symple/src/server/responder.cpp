///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/server.h"
#include "detail.h"
#include "../protocol.h"
#include "icy/logger.h"


namespace icy {
namespace smpl {


class Server::Responder : public http::ServerResponder
{
public:
    Responder(http::ServerConnection& conn, Server& server)
        : http::ServerResponder(conn)
        , _server(server)
        , _tempPeer(conn)
    {
        _authTimer.setTimeout(10000);
        _authTimer.start();
        _authTimer.Timeout += [this]() {
            if (!_authenticated) {
                LWarn("Auth timeout, closing connection");
                sendError(408, "Authentication timeout");
                connection().close();
            }
        };
    }

    void onPayload(const MutableBuffer& buffer) override
    {
        bool shouldClose = false;

        if (_server._opts.maxMessageSize > 0 &&
            buffer.size() > _server._opts.maxMessageSize) {
            LWarn("Message too large (", buffer.size(), " bytes), closing");
            sendError(413, "Message too large");
            shouldClose = true;
        }

        json::Value msg;
        std::string type;

        if (!shouldClose) {
            std::string str(bufferCast<const char*>(buffer), buffer.size());
            try {
                msg = json::Value::parse(str);
            }
            catch (const std::exception& e) {
                LWarn("JSON parse error: ", e.what());
                return;
            }
            type = msg.value("type", "");
        }

        if (!shouldClose) {
            std::unique_lock<std::mutex> lock(_server._mutex);
            auto* peer = _server._peerRegistry->findByConnection(connection());

            if (!peer) {
                if (type == "auth") {
                    _server.onAuth(_tempPeer, msg, lock);
                    if (_tempPeer.authenticated()) {
                        _authenticated = true;
                        _authTimer.stop();
                    }
                }
                else {
                    sendError(401, "Not authenticated");
                    shouldClose = true;
                }
            }
            else if (!peer->checkRate()) {
                LWarn("Rate limit exceeded for peer ", peer->id());
                sendError(429, "Rate limit exceeded");
                shouldClose = true;
            }
            else if (type == "message" || type == "presence" ||
                     type == "command" || type == "event") {
                _server.onMessage(*peer, std::move(msg));
            }
            else if (type == "join") {
                _server.onJoin(*peer, msg.value("room", ""));
            }
            else if (type == "leave") {
                _server.onLeave(*peer, msg.value("room", ""));
            }
            else if (type == "close") {
                shouldClose = true;
            }
        }

        if (shouldClose)
            connection().close();
    }

    void onClose() override
    {
        _authTimer.stop();
        if (_server._shuttingDown)
            return;
        std::unique_lock<std::mutex> lock(_server._mutex);
        if (auto* peer = _server._peerRegistry->findByConnection(connection()))
            _server.onDisconnect(*peer, lock);
    }

    void sendError(int status, const std::string& message)
    {
        auto err = proto::makeError(status, message);
        auto str = err.dump();
        try {
            connection().send(str.c_str(), str.size(), http::ws::Text);
        }
        catch (...) {}
    }

    Server& _server;
    ServerPeer _tempPeer;
    Timer _authTimer;
    bool _authenticated = false;
};


class Server::Factory : public http::ServerConnectionFactory
{
public:
    explicit Factory(Server& server)
        : _server(server)
    {
    }

    std::unique_ptr<http::ServerResponder> createResponder(
        http::ServerConnection& conn) override
    {
        if (!conn.upgraded()) {
            if (_server._httpFallback)
                return _server._httpFallback->createResponder(conn);
            return nullptr;
        }

        if (_server._opts.maxConnections > 0 &&
            _server._http->connectionCount() > static_cast<size_t>(_server._opts.maxConnections)) {
            LWarn("Max connections reached (", _server._opts.maxConnections, "), rejecting");
            auto err = proto::makeError(503, "Server at capacity");
            auto str = err.dump();
            try {
                conn.send(str.c_str(), str.size(), http::ws::Text);
            }
            catch (...) {}
            conn.close();
            return nullptr;
        }

        return std::make_unique<Server::Responder>(conn, _server);
    }

    Server& _server;
};


void Server::start(const Options& opts)
{
    start(opts, nullptr);
}


void Server::start(const Options& opts,
                   std::unique_ptr<http::ServerConnectionFactory> httpFactory)
{
    _opts = opts;
    _httpFallback = std::move(httpFactory);

    _http = std::make_unique<http::Server>(
        opts.host, opts.port,
        _loop,
        std::make_unique<Factory>(*this));

    _http->start();

    LInfo("Symple server listening on ", opts.host, ":", opts.port);
}


} // namespace smpl
} // namespace icy
