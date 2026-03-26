///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/client.h"
#include "../protocol.h"
#include "detail.h"
#include "roomstate.h"
#include "icy/logger.h"

#include <stdexcept>


namespace icy {
namespace smpl {


Client::Client()
    : _data(std::make_unique<ClientData>(Options{}, uv::defaultLoop()))
{
    _data->reconnectTimer.Timeout += [this]() {
        doConnect();
    };
}


Client::Client(const Options& options, uv::Loop* loop)
    : _data(std::make_unique<ClientData>(options, loop))
{
    _data->reconnectTimer.Timeout += [this]() {
        doConnect();
    };
}


Client::~Client()
{
    _data->reconnectTimer.stop();
    close();
}


void Client::connect()
{
    auto& data = *_data;
    if (data.options.host.empty())
        throw std::runtime_error("Symple client host must not be empty");
    if (data.options.user.empty())
        throw std::runtime_error("Symple client user must not be empty");

    data.reconnectCount = 0;
    doConnect();
}


void Client::doConnect()
{
    auto& data = *_data;
    reset();
    setState(this, ClientState::Connecting);

    std::string url = buildUrl();
    LDebug("Connecting to ", url);

    data.ws = http::createConnectionT<http::ClientConnection>(http::URL(url), data.loop);

    data.ws->Connect += [this]() {
        auto& data = *_data;
        LDebug("WebSocket ready, sending auth");
        setState(this, ClientState::Authenticating);
        sendJson(proto::makeAuth(
            data.options.user,
            data.options.name,
            data.options.token,
            data.options.type,
            data.desiredRooms));
    };

    data.ws->Payload += [this](const MutableBuffer& buf) {
        std::string data(bufferCast<const char*>(buf), buf.size());
        onSocketRecv(data);
    };

    data.ws->Error += slot(this, &Client::onTransportError);

    data.ws->Close += [this](http::Connection&) {
        onSocketClose();
    };

    data.ws->submit();
}


void Client::close()
{
    auto& data = *_data;
    if (data.closing)
        return;
    data.closing = true;

    data.reconnectTimer.stop();

    if (data.ws) {
        if (isOnline()) {
            try {
                sendJson(proto::makeClose());
            }
            catch (...) {}
        }

        data.ws->close();
        data.ws.reset();
    }

    reset();

    if (!stateEquals(ClientState::Closed))
        setState(this, ClientState::Closed);

    data.closing = false;
}


void Client::onTransportError(const icy::Error& error)
{
    onSocketError(error.message.empty() ? "WebSocket transport error" : error.message);
}


void Client::onSocketClose()
{
    auto& data = *_data;
    LInfo("WebSocket disconnected");

    if (data.closing || stateEquals(ClientState::Closed) || stateEquals(ClientState::Error))
        return;

    bool wasOnline = data.wasOnline;
    reset();
    setState(this, ClientState::Closed);

    if (wasOnline && data.options.reconnection)
        startReconnect();
}


void Client::onSocketError(const std::string& error)
{
    LError("WebSocket error: ", error);
    setError(error);
}


void Client::startReconnect()
{
    auto& data = *_data;
    if (data.options.reconnectAttempts > 0 &&
        data.reconnectCount >= data.options.reconnectAttempts) {
        LWarn("Max reconnect attempts reached");
        return;
    }

    data.reconnectCount++;
    LInfo("Reconnecting (attempt ", data.reconnectCount, ")...");

    data.reconnectTimer.setTimeout(data.options.reconnectDelay);
    data.reconnectTimer.start();
}


void Client::reset()
{
    auto& data = *_data;
    data.roster.clear();
    detail::resetTransientRoomState(data.currentRooms,
                                    data.pendingJoins,
                                    data.pendingLeaves);
    data.announceStatus = 0;
    data.ourID.clear();
    data.wasOnline = false;
}


void Client::syncDesiredRooms()
{
    auto& data = *_data;
    if (!data.ws || !isOnline())
        return;

    for (const auto& room : detail::collectJoinRequests(data.desiredRooms,
                                                        data.currentRooms,
                                                        data.pendingJoins)) {
        if (sendJson(proto::makeRoomMessage("join", room)) >= 0)
            data.pendingJoins.insert(room);
    }
}


int Client::sendJson(const json::Value& msg)
{
    auto& data = *_data;
    if (!data.ws)
        return -1;

    auto str = msg.dump();
    return data.ws->send(str.c_str(), str.size(), http::ws::Text);
}


std::string Client::buildUrl() const
{
    const auto& options = _data->options;
    std::string scheme = options.secure ? "wss" : "ws";
    return scheme + "://" + options.host + ":" +
           std::to_string(options.port) + "/";
}


} // namespace smpl
} // namespace icy
