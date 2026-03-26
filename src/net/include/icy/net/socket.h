///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "icy/base.h"
#include "icy/buffer.h"
#include "icy/handle.h"
#include "icy/net/address.h"
#include "icy/net/net.h"
#include "icy/net/socketadapter.h"
#include "icy/packetstream.h"

#include "uv.h"

#include <any>
#include <stdexcept>
#include <string_view>


namespace icy {
namespace net {


/// Creates a socket of type SocketT wrapped in a shared_ptr.
///
/// The socket is automatically destroyed when the last shared_ptr owner releases it.
/// @tparam SocketT  The concrete socket type to instantiate (e.g. TCPSocket, UDPSocket).
/// @param loop      Event loop to associate with the socket; defaults to the default loop.
/// @return A shared_ptr owning the newly created socket.
template <class SocketT>
inline std::shared_ptr<SocketT> makeSocket(uv::Loop* loop = uv::defaultLoop())
{
    // std::shared_ptr<SocketT>(new SocketT(loop), deleter::Deferred<SocketT>());
    return std::make_shared<SocketT>(loop);
}


/// Base socket implementation from which all sockets derive.
class Net_API Socket : public SocketAdapter
{
public:
    using Ptr = std::shared_ptr<Socket>;
    using Vec = std::vector<Ptr>;

    Socket() = default;
    virtual ~Socket() noexcept = default;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&&) = delete;
    Socket& operator=(Socket&&) = delete;

    /// Connects to the given peer IP address.
    ///
    /// Throws an exception if the address is malformed.
    /// Connection errors can be handled via the Error signal.
    virtual void connect(const Address& address) = 0;

    /// Resolves and connects to the given host address.
    ///
    /// Throws an Exception if the host is malformed.
    /// Since the DNS callback is asynchronous implementations need
    /// to listen for the Error signal for handling connection errors.
    virtual void connect(std::string_view host, uint16_t port) = 0;

    /// Bind a local address to the socket.
    /// The address may be IPv4 or IPv6 (if supported).
    ///
    /// Throws an Exception on error.
    virtual void bind(const Address& address, unsigned flags = 0) = 0;

    /// Listens the socket on the given address.
    ///
    /// Throws an Exception on error.
    virtual void listen(int backlog = 64) { (void)backlog; }

    /// Sends the shutdown packet which should result is socket
    /// closure via callback.
    [[nodiscard]] virtual bool shutdown()
    {
        throw std::logic_error("Socket::shutdown not implemented by protocol");
        return false;
    };

    /// Sends an owned payload buffer to the connected peer.
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, int flags = 0) = 0;
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, const Address& peerAddress, int flags = 0) = 0;

    /// Closes the underlying socket.
    virtual void close() = 0;

    /// The locally bound address.
    ///
    /// This function will not throw.
    /// A Wildcard 0.0.0.0:0 address is returned if
    /// the socket is closed or invalid.
    virtual Address address() const = 0;

    /// The connected peer address.
    ///
    /// This function will not throw.
    /// A Wildcard 0.0.0.0:0 address is returned if
    /// the socket is closed or invalid.
    virtual Address peerAddress() const = 0;

    /// The transport protocol: TCP, UDP or SSLTCP.
    virtual net::TransportType transport() const = 0;

    /// Sets the socket error.
    ///
    /// Setting the error will result in socket closure.
    virtual void setError(const icy::Error& err) = 0;

    /// Return the socket error if any.
    virtual const icy::Error& error() const = 0;

    /// Returns true if the native socket handle is closed.
    virtual bool closed() const = 0;

    /// Returns the socket event loop.
    virtual uv::Loop* loop() const = 0;

    /// Optional client data.
    ///
    /// The value is empty on initialization.
    std::any opaque;

protected:
    /// Initializes the underlying socket context.
    virtual void init() = 0;

    /// Resets the socket context for reuse.
    virtual void reset() = 0;

    int _af{AF_UNSPEC};
};


//
// Packet Info
//


/// Provides information about packets emitted from a socket.
/// See SocketPacket.
struct PacketInfo : public IPacketInfo
{
    /// The source socket
    Socket::Ptr socket;

    /// The originating peer address.
    /// For TCP this will always be connected address.
    Address peerAddress;

    /// Constructs PacketInfo with the originating socket and peer address.
    /// @param socket      Shared pointer to the socket that received the packet.
    /// @param peerAddress Address of the remote peer that sent the packet.
    PacketInfo(const Socket::Ptr& socket, const Address& peerAddress)
        : socket(socket)
        , peerAddress(peerAddress)
    {
    }

    /// Copy constructor.
    /// @param r Source PacketInfo to copy from.
    PacketInfo(const PacketInfo& r)
        : socket(r.socket)
        , peerAddress(r.peerAddress)
    {
    }

    /// Returns a heap-allocated copy of this PacketInfo.
    virtual std::unique_ptr<IPacketInfo> clone() const { return std::make_unique<PacketInfo>(*this); }

    virtual ~PacketInfo() noexcept = default;
};


//
// Socket Packet
//


/// Default packet type emitted by sockets.
///
/// SocketPacket carries the remote peer address plus a borrowed view of the
/// received byte buffer for zero-copy processing inside the receive callback.
///
/// The referenced packet buffer lifetime is only guaranteed
/// for the duration of the receiver callback.
class Net_API SocketPacket : public RawPacket
{
public:
    /// Constructs a SocketPacket wrapping the received buffer.
    ///
    /// The buffer data pointer remains valid only for the duration of the
    /// enclosing receive callback; do not retain references beyond that scope.
    /// @param socket      Shared pointer to the receiving socket.
    /// @param buffer      View of the raw received bytes.
    /// @param peerAddress Address of the remote sender.
    SocketPacket(const Socket::Ptr& socket, const MutableBuffer& buffer, const Address& peerAddress)
        : RawPacket(bufferCast<char*>(buffer), buffer.size(), 0,
                    std::make_unique<PacketInfo>(socket, peerAddress))
    {
    }

    /// Copy constructor; shares the underlying buffer reference.
    /// @param that Source SocketPacket to copy.
    SocketPacket(const SocketPacket& that)
        : RawPacket(that)
    {
    }

    /// Returns the PacketInfo for this socket packet.
    /// @return Pointer to the associated PacketInfo (never null for a valid packet).
    PacketInfo* packetInfo() const
    {
        return static_cast<PacketInfo*>(info.get());
    }

    virtual ~SocketPacket() noexcept = default;

    /// Prints a one-line description of the packet to @p os.
    /// @param os Output stream to write to.
    virtual void print(std::ostream& os) const
    {
        os << className() << ": " << packetInfo()->peerAddress << '\n';
    }

    /// Returns a heap-allocated copy of this SocketPacket.
    virtual std::unique_ptr<IPacket> clone() const { return std::make_unique<SocketPacket>(*this); }

    /// Not supported; always throws std::logic_error.
    virtual ssize_t read(const ConstBuffer&)
    {
        throw std::logic_error("SocketPacket::read not supported; write only");
        return 0;
    }

    /// Appends the packet payload to @p buf.
    /// @param buf Destination buffer to append raw bytes to.
    virtual void write(Buffer& buf) const
    {
        // buf.append(data(), size());
        buf.insert(buf.end(), data(), data() + size());
    }

    /// @return The string "SocketPacket".
    virtual const char* className() const { return "SocketPacket"; }
};


} // namespace net
} // namespace icy


/// @}
