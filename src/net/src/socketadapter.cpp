///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/socketadapter.h"
#include "scy/net/socket.h"

#include <iterator>


using std::endl;


namespace scy {
namespace net {


SocketAdapter::SocketAdapter(SocketAdapter* sender)
    : _sender(sender)
{
    // LTrace("Create")
    assert(sender != this);
}


SocketAdapter::~SocketAdapter()
{
    // LTrace("Destroy")
    // assert(_receivers.empty());
}


ssize_t SocketAdapter::send(const char* data, size_t len, int flags)
{
    assert(_sender); // should have output adapter if default impl is used
    if (!_sender)
        return -1;
    return _sender->send(data, len, flags);
}


ssize_t SocketAdapter::send(const char* data, size_t len, const Address& peerAddress, int flags)
{
    assert(_sender); // should have output adapter if default impl is used
    if (!_sender)
        return -1;
    return _sender->send(data, len, peerAddress, flags);
}


ssize_t SocketAdapter::sendPacket(const IPacket& packet, int flags)
{
    // Try to cast as RawPacket so we can send without copying any data.
    auto raw = dynamic_cast<const RawPacket*>(&packet);
    if (raw)
        return send((const char*)raw->data(), raw->size(), flags);

    // Dynamically generated packets need to be written to a
    // temp buffer for sending.
    else {
        Buffer buf;
        packet.write(buf);
        return send(buf.data(), buf.size(), flags);
    }
}


ssize_t SocketAdapter::sendPacket(const IPacket& packet, const Address& peerAddress, int flags)
{
    // Try to cast as RawPacket so we can send without copying any data.
    auto raw = dynamic_cast<const RawPacket*>(&packet);
    if (raw)
        return send((const char*)raw->data(), raw->size(), peerAddress, flags);

    // Dynamically generated packets need to be written to a
    // temp buffer for sending.
    else {
        Buffer buf;
        buf.reserve(1024);
        packet.write(buf);
        return send(buf.data(), buf.size(), peerAddress, flags);
    }
}


void SocketAdapter::sendPacket(IPacket& packet)
{
    size_t res = sendPacket(packet, 0);
    if (res < 0)
        throw std::runtime_error("Invalid socket operation");
}


void SocketAdapter::onSocketConnect(Socket& socket)
{
    try {
        cleanupReceivers();
        int current = int(_receivers.size() - 1);
        while (current >= 0) {
            auto ref = _receivers[current--];
            if (ref->alive)
                ref->ptr->onSocketConnect(socket);
        }
    }
    catch (StopPropagation&) {
    }
}


void SocketAdapter::onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
{
    try {
        cleanupReceivers();
        int current = int(_receivers.size() - 1);
        while (current >= 0) {
            auto ref = _receivers[current--];
            if (ref->alive)
                ref->ptr->onSocketRecv(socket, buffer, peerAddress);
        }
    }
    catch (StopPropagation&) {
    }
}


void SocketAdapter::onSocketError(Socket& socket, const scy::Error& error)
{
    try {
        cleanupReceivers();
        int current = int(_receivers.size() - 1);
        while (current >= 0) {
            auto ref = _receivers[current--];
            if (ref->alive)
                ref->ptr->onSocketError(socket, error);
        }
    }
    catch (StopPropagation&) {
    }
}


void SocketAdapter::onSocketClose(Socket& socket)
{
    try {
        cleanupReceivers();
        int current = int(_receivers.size() - 1);
        while (current >= 0) {
            auto ref = _receivers[current--];
            if (ref->alive) {
                ref->ptr->onSocketClose(socket);
            }
        }
    }
    catch (StopPropagation&) {
    }
}


void SocketAdapter::setSender(SocketAdapter* adapter)
{
    assert(adapter != this);
    if (_sender == adapter)
        return;
    _sender = adapter;
}


bool SocketAdapter::hasReceiver(SocketAdapter* adapter)
{
    for (auto& receiver : _receivers) {
        if (receiver->ptr == adapter)
            return true;
    }
    return false;
}


void SocketAdapter::addReceiver(SocketAdapter* adapter)
{
    assert(adapter->priority <= 100);
    assert(adapter != this);
    if (hasReceiver(adapter))
        return;

    // Note that we insert new adapters in the back of the queue,
    // and iterate in reverse to ensure calling order is preserved.
    _dirty = true;
	auto ptrRef = std::make_shared<Ref>();
	ptrRef->ptr = adapter;
	ptrRef->alive = true;
    _receivers.push_back(ptrRef);
    // _receivers.insert(_receivers.begin(), new Ref{ adapter, false }); // insert front
    // std::sort(_receivers.begin(), _receivers.end(),
    //     [](SocketAdapter const& l, SocketAdapter const& r) {
    //     return l.priority > r.priority; });
}


void SocketAdapter::removeReceiver(SocketAdapter* adapter)
{
    assert(adapter != this);
    auto it = std::find_if(_receivers.begin(), _receivers.end(),
        [&](const Ref::ptr_t& ref) { return ref->ptr == adapter; });
    if (it != _receivers.end()) { (*it)->alive = false; }
}


void SocketAdapter::cleanupReceivers()
{
    if (!_dirty) return;
    for (auto it = _receivers.begin(); it != _receivers.end();) {
        auto ref = *it;
        if (!ref->alive) {
            //delete ref;
            it = _receivers.erase(it);
        }
        else ++it;
    }
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
        [](const Ref::ptr_t& ref) { return ref->ptr; });
    return items;
}


} // namespace net
} // namespace scy


/// @\}
