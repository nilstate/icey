///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "icy/base.h"
#include "icy/buffer.h"
#include "icy/http/connection.h"
#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/net/socket.h"
#include "icy/net/socketemitter.h"
#include "icy/net/tcpsocket.h"
#include "icy/random.h"


namespace icy {
namespace http {
namespace ws {


/// WebSocket endpoint mode.
enum class Mode
{
    ServerSide, ///< Server-side WebSocket.
    ClientSide  ///< Client-side WebSocket.
};


/// Frame header flags.
enum class FrameFlags
{
    Fin = 0x80,  ///< FIN bit: final fragment of a multi-fragment message.
    Rsv1 = 0x40, ///< Reserved for future use. Must be zero.
    Rsv2 = 0x20, ///< Reserved for future use. Must be zero.
    Rsv3 = 0x10, ///< Reserved for future use. Must be zero.
};


/// Frame header opcodes.
enum class Opcode
{
    Continuation = 0x00, ///< Continuation frame.
    Text = 0x01,         ///< Text frame.
    Binary = 0x02,       ///< Binary frame.
    Close = 0x08,        ///< Close connection.
    Ping = 0x09,         ///< Ping frame.
    Pong = 0x0a,         ///< Pong frame.
    Bitmask = 0x0f       ///< Bit mask for opcodes.
};


/// Combined header flags and opcodes for identifying
/// the payload type of sent frames.
enum SendFlags
{
    Text = unsigned(ws::FrameFlags::Fin) | unsigned(ws::Opcode::Text),
    Binary = unsigned(ws::FrameFlags::Fin) | unsigned(ws::Opcode::Binary)
};


/// StatusCodes for CLOSE frames sent with shutdown().
enum class CloseStatusCode
{
    NormalClose = 1000,
    EndpointGoingAway = 1001,
    ProtocolError = 1002,
    PayloadNotAcceptable = 1003,
    Reserved = 1004,
    ReservedNoStatusCode = 1005,
    ReservedAbnormalClose = 1006,
    MalformedPayload = 1007,
    PolicyViolation = 1008,
    PayloadTooBig = 1009,
    ExtensionRequired = 1010,
    UnexpectedCondition = 1011,
    ReservedTLSFailure = 1015
};


/// These error codes can be obtained from WebSocket exceptions
/// to determine the exact cause of the error.
enum class ErrorCode
{
    NoHandshake = 1,                 ///< No Connection: Upgrade or Upgrade: websocket header in handshake request.
    HandshakeNoVersion = 2,          ///< No Sec-WebSocket-Version header in handshake request.
    HandshakeUnsupportedVersion = 3, ///< Unsupported WebSocket version requested by client.
    HandshakeNoKey = 4,              ///< No Sec-WebSocket-Key header in handshake request.
    HandshakeAccept = 5,             ///< No Sec-WebSocket-Accept header or wrong value.
    Unauthorized = 6,                ///< The server rejected the username or password for authentication.
    PayloadTooBig = 10,              ///< Payload too big for supplied buffer.
    IncompleteFrame = 11,            ///< Incomplete frame received.
    InvalidRsvBits = 12,             ///< RSV bits set without extension negotiation.
    InvalidOpcode = 13,              ///< Unknown or reserved opcode received.
    UnmaskedClientFrame = 14         ///< Client-to-server frame not masked (RFC 6455 violation).
};


// Keep old names as constants for backward compatibility
static constexpr auto ServerSide = Mode::ServerSide;
static constexpr auto ClientSide = Mode::ClientSide;

inline constexpr char ProtocolGuid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/// The WebSocket protocol version supported (13).
inline constexpr char ProtocolVersion[] = "13";


//
// WebSocket Framer
//

/// This class implements a WebSocket parser according
/// to the WebSocket protocol described in RFC 6455.
class HTTP_API WebSocketFramer
{
public:
    /// Creates a Socket using the given Socket.
    WebSocketFramer(ws::Mode mode);

    virtual ~WebSocketFramer();

    /// Writes a WebSocket protocol frame from the given data.
    virtual size_t writeFrame(const char* data, size_t len, int flags, BitWriter& frame);

    /// Reads a single WebSocket frame from the given buffer (frame).
    ///
    /// The actual payload length is returned, and the beginning of the
    /// payload buffer will be assigned in the second (payload) argument.
    /// No data is copied.
    ///
    /// If the frame is invalid or too big an exception will be thrown.
    /// Return true when the handshake has completed successfully.
    virtual uint64_t readFrame(BitReader& frame, char*& payload);

    [[nodiscard]] bool handshakeComplete() const;

    //
    /// Server side

    void acceptServerRequest(http::Request& request, http::Response& response);

    //
    /// Client side

    /// Appends the WS hanshake HTTP request hearers.
    void createClientHandshakeRequest(http::Request& request);

    /// Checks the veracity the HTTP handshake response.
    /// Returns true on success, false if the request should
    /// be resent (in case of authentication), or throws on error.
    bool checkClientHandshakeResponse(http::Response& response);

    /// Verifies the handshake response or thrown and exception.
    void completeClientHandshake(http::Response& response);

protected:
    /// Returns the frame flags of the most recently received frame.
    /// Set by readFrame()
    [[nodiscard]] int frameFlags() const;

    /// Returns true if the payload must be masched.
    /// Used by writeFrame()
    [[nodiscard]] bool mustMaskPayload() const;

    [[nodiscard]] ws::Mode mode() const;

    enum
    {
        FRAME_FLAG_MASK = 0x80,
        MAX_HEADER_LENGTH = 14
    };

private:
    ws::Mode _mode;
    int _frameFlags;
    int _headerState;
    bool _maskPayload;
    Random _rnd;
    std::string _key; // client handshake key

    // Fragmentation state (RFC 6455 Section 5.4)
    bool _fragmented{false};       ///< Currently receiving a fragmented message
    int _fragmentOpcode{0};        ///< Opcode of the first frame in the fragment sequence
    Buffer _fragmentBuffer;        ///< Accumulated payload from continuation frames

    // Partial frame buffering (when TCP segment doesn't contain a full frame)
    Buffer _incompleteFrame;       ///< Buffer for incomplete frame data across TCP segments

    friend class WebSocketAdapter;

    // Allow direct member access for unit testing
    friend void wsFramerTestAccess(WebSocketFramer& f, int state);
    friend int wsFramerGetFlags(const WebSocketFramer& f);
};


//
// WebSocket Adapter
//


/// WebSocket protocol adapter for both client and server endpoints.
class HTTP_API WebSocketAdapter : public net::SocketEmitter
{
public:
    WebSocketAdapter(const net::Socket::Ptr& socket, ws::Mode mode,
                     http::Request& request, http::Response& response);

    virtual ssize_t send(const char* data, size_t len, int flags = 0) override;                               // flags = ws::Text || ws::Binary
    virtual ssize_t send(const char* data, size_t len, const net::Address& peerAddr, int flags = 0) override; // flags = ws::Text || ws::Binary

    virtual bool shutdown(uint16_t statusCode, const std::string& statusMessage);

    /// Pointer to the underlying socket.
    /// Sent data will be proxied to this socket.
    net::Socket::Ptr socket;

    //
    /// Client side

    virtual void sendClientRequest();
    virtual void handleClientResponse(const MutableBuffer& buffer, const net::Address& peerAddr);

    //
    /// Server side

    virtual void handleServerRequest(const MutableBuffer& buffer, const net::Address& peerAddr);

    virtual bool onSocketConnect(net::Socket& socket) override;
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress) override;
    virtual bool onSocketClose(net::Socket& socket) override;

    virtual void onHandshakeComplete();

protected:
    virtual ~WebSocketAdapter();

    friend class WebSocketFramer;

    WebSocketFramer framer;
    http::Request& _request;
    http::Response& _response;
};


//
// WebSocket
//


/// Standalone WebSocket class.
class HTTP_API WebSocket : public WebSocketAdapter
{
public:
    using Vec = std::vector<WebSocket>;

    /// Creates the WebSocket with the given Socket.
    /// The Socket should be a TCPSocket or a SSLSocket,
    /// depending on the protocol used (ws or wss).
    WebSocket(const net::Socket::Ptr& socket);

    virtual ~WebSocket();

    [[nodiscard]] http::Request& request();
    [[nodiscard]] http::Response& response();

protected:
    http::Request _request;
    http::Response _response;
};


//
// WebSocket Connection Adapter
//


/// WebSocket class which belongs to a HTTP Connection.
class HTTP_API ConnectionAdapter : public WebSocketAdapter
{
public:
    ConnectionAdapter(Connection* connection, ws::Mode mode);
    virtual ~ConnectionAdapter();

    virtual void onHandshakeComplete();

protected:
    Connection* _connection;
};


} // namespace ws
} // namespace http
} // namespace icy


/// @}
