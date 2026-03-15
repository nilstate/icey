///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/socketemitter.h"


using std::endl;


namespace scy {
namespace net {


SocketEmitter::SocketEmitter(const Socket::Ptr& socket)
    : SocketAdapter(socket.get())
    , impl(socket)
{
    if (impl)
        impl->addReceiver(this);
}


SocketEmitter::SocketEmitter(const SocketEmitter& that)
    : SocketAdapter(that)
    , Connect(that.Connect)
    , Recv(that.Recv)
    , Error(that.Error)
    , Close(that.Close)
    , impl(that.impl)
{
    if (impl)
        impl->addReceiver(this);
}


SocketEmitter::~SocketEmitter() noexcept
{
    if (impl)
        impl->removeReceiver(this);
}


void SocketEmitter::swap(const Socket::Ptr& socket)
{
    if (impl)
        throw std::logic_error("SocketEmitter: already initialized");
    if (impl)
        impl->removeReceiver(this);
    if (socket)
        socket->addReceiver(this);
    impl = socket;
}


void SocketEmitter::addReceiver(SocketAdapter* adapter)
{
    // LTrace("Add receiver: ", adapter);
    if (adapter->priority > 100)
        throw std::invalid_argument("SocketEmitter: adapter priority exceeds 100");
    Connect.attach([adapter](Socket& s) { return adapter->onSocketConnect(s); }, adapter, -1, adapter->priority);
    Recv.attach([adapter](Socket& s, const MutableBuffer& b, const Address& a) { return adapter->onSocketRecv(s, b, a); }, adapter, -1, adapter->priority);
    Error.attach([adapter](Socket& s, const scy::Error& e) { return adapter->onSocketError(s, e); }, adapter, -1, adapter->priority);
    Close.attach([adapter](Socket& s) { return adapter->onSocketClose(s); }, adapter, -1, adapter->priority);
}


void SocketEmitter::removeReceiver(SocketAdapter* adapter)
{
    // LTrace("Remove receiver: ", adapter);
    Connect.detach(adapter);
    Recv.detach(adapter);
    Error.detach(adapter);
    Close.detach(adapter);

    // Connect -= slot(adapter, &net::SocketAdapter::onSocketConnect);
    // Recv -= slot(adapter, &net::SocketAdapter::onSocketRecv);
    // Error -= slot(adapter, &net::SocketAdapter::onSocketError);
    // Close -= slot(adapter, &net::SocketAdapter::onSocketClose);
}


bool SocketEmitter::onSocketConnect(Socket& socket)
{
    if (SocketAdapter::onSocketConnect(socket))
        return true;
    return Connect.emit(socket);
}


bool SocketEmitter::onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
{
    if (SocketAdapter::onSocketRecv(socket, buffer, peerAddress))
        return true;
    return Recv.emit(socket, buffer, peerAddress);
}


bool SocketEmitter::onSocketError(Socket& socket, const scy::Error& error)
{
    if (SocketAdapter::onSocketError(socket, error))
        return true;
    return Error.emit(socket, error);
}


bool SocketEmitter::onSocketClose(Socket& socket)
{
    if (SocketAdapter::onSocketClose(socket))
        return true;
    return Close.emit(socket);
}


} // namespace net
} // namespace scy


/// @\}
