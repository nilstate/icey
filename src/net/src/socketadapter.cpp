///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "icy/net/socketadapter.h"
#include "icy/net/socket.h"

#include <iterator>




namespace icy {
namespace net {


SocketAdapter::SocketAdapter(SocketAdapter* sender)
    : _sender(sender)
{
    // LTrace("Create");
    if (sender == this)
        throw std::logic_error("SocketAdapter: cannot set self as sender");
}


SocketAdapter::~SocketAdapter() noexcept
{
    // LTrace("Destroy");
    // assert(_receivers.empty());
}


ssize_t SocketAdapter::send(const char* data, size_t len, int flags)
{
    if (!_sender)
        return -1;
    return _sender->send(data, len, flags);
}


ssize_t SocketAdapter::send(const char* data, size_t len, const Address& peerAddress, int flags)
{
    if (!_sender)
        return -1;
    return _sender->send(data, len, peerAddress, flags);
}


ssize_t SocketAdapter::sendOwned(Buffer&& buffer, int flags)
{
    if (!_sender)
        return -1;
    return _sender->sendOwned(std::move(buffer), flags);
}


ssize_t SocketAdapter::sendOwned(Buffer&& buffer, const Address& peerAddress, int flags)
{
    if (!_sender)
        return -1;
    return _sender->sendOwned(std::move(buffer), peerAddress, flags);
}


ssize_t SocketAdapter::sendPacket(const IPacket& packet, int flags)
{
    Buffer buf;
    buf.reserve(packet.size());
    packet.write(buf);
    return sendOwned(std::move(buf), flags);
}


ssize_t SocketAdapter::sendPacket(const IPacket& packet, const Address& peerAddress, int flags)
{
    Buffer buf;
    buf.reserve(packet.size());
    packet.write(buf);
    return sendOwned(std::move(buf), peerAddress, flags);
}


void SocketAdapter::sendPacket(IPacket& packet)
{
    ssize_t res = sendPacket(packet, 0);
    if (res < 0)
        throw std::runtime_error("Invalid socket operation");
}


bool SocketAdapter::onSocketConnect(Socket& socket)
{
    cleanupReceivers();
    if (_receivers.size() == 1 && _receivers[0].alive)
        return _receivers[0].ptr->onSocketConnect(socket);
    int current = int(_receivers.size() - 1);
    while (current >= 0) {
        auto& ref = _receivers[current--];
        if (ref.alive && ref.ptr->onSocketConnect(socket))
            return true;
    }
    return false;
}


bool SocketAdapter::onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
{
    cleanupReceivers();
    if (_receivers.size() == 1 && _receivers[0].alive)
        return _receivers[0].ptr->onSocketRecv(socket, buffer, peerAddress);
    int current = int(_receivers.size() - 1);
    while (current >= 0) {
        auto& ref = _receivers[current--];
        if (ref.alive && ref.ptr->onSocketRecv(socket, buffer, peerAddress))
            return true;
    }
    return false;
}


bool SocketAdapter::onSocketError(Socket& socket, const icy::Error& error)
{
    cleanupReceivers();
    if (_receivers.size() == 1 && _receivers[0].alive)
        return _receivers[0].ptr->onSocketError(socket, error);
    int current = int(_receivers.size() - 1);
    while (current >= 0) {
        auto& ref = _receivers[current--];
        if (ref.alive && ref.ptr->onSocketError(socket, error))
            return true;
    }
    return false;
}


bool SocketAdapter::onSocketClose(Socket& socket)
{
    cleanupReceivers();
    if (_receivers.size() == 1 && _receivers[0].alive)
        return _receivers[0].ptr->onSocketClose(socket);
    int current = int(_receivers.size() - 1);
    while (current >= 0) {
        auto& ref = _receivers[current--];
        if (ref.alive && ref.ptr->onSocketClose(socket))
            return true;
    }
    return false;
}


void SocketAdapter::setSender(SocketAdapter* adapter)
{
    if (adapter == this)
        throw std::logic_error("SocketAdapter: cannot set self as sender");
    if (_sender == adapter)
        return;
    _sender = adapter;
}


bool SocketAdapter::hasReceiver(SocketAdapter* adapter)
{
    for (auto& ref : _receivers) {
        if (ref.ptr == adapter)
            return true;
    }
    return false;
}


void SocketAdapter::addReceiver(SocketAdapter* adapter)
{
    if (adapter->priority > 100)
        throw std::invalid_argument("priority must be <= 100");
    if (adapter == this)
        throw std::logic_error("SocketAdapter: cannot add self");
    if (hasReceiver(adapter))
        return;

    _dirty = true;
    _receivers.push_back(Ref{adapter, true});
}


void SocketAdapter::removeReceiver(SocketAdapter* adapter)
{
    if (adapter == this)
        return;
    for (auto& ref : _receivers) {
        if (ref.ptr == adapter) {
            ref.alive = false;
            _dirty = true;
            return;
        }
    }
}


void SocketAdapter::cleanupReceivers()
{
    if (!_dirty)
        return;
    _receivers.erase(
        std::remove_if(_receivers.begin(), _receivers.end(),
                       [](const Ref& ref) { return !ref.alive; }),
        _receivers.end());
    _dirty = false;
}


SocketAdapter* SocketAdapter::sender()
{
    return _sender;
}


std::vector<SocketAdapter*> SocketAdapter::receivers()
{
    std::vector<SocketAdapter*> items;
    std::transform(_receivers.begin(), _receivers.end(), std::back_inserter(items),
                   [](const Ref& ref) { return ref.ptr; });
    return items;
}


} // namespace net
} // namespace icy


/// @\}
