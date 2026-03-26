///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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

#include <stdexcept>
#include <string>


namespace icy {
namespace http {
/// WebSocket framing, handshakes, and connection helpers.
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
    UnmaskedClientFrame = 14,        ///< Client-to-server frame not masked (RFC 6455 violation).
    ProtocolViolation = 15           ///< General RFC 6455 protocol violation.
};


enum class CloseState : uint8_t
{
    Open,
    CloseSent,
    CloseReceived,
    Closed,
};


class HTTP_API WebSocketException : public std::runtime_error
{
public:
    WebSocketException(ErrorCode code, std::string message, uint16_t closeStatus = 0)
        : std::runtime_error(std::move(message))
        , _code(code)
        , _closeStatus(closeStatus)
    {
    }

    [[nodiscard]] ErrorCode code() const { return _code; }
    [[nodiscard]] bool hasCloseStatus() const { return _closeStatus != 0; }
    [[nodiscard]] uint16_t closeStatus() const { return _closeStatus; }

private:
    ErrorCode _code;
    uint16_t _closeStatus;
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

/// WebSocket frame encoder/decoder and handshake validator for RFC 6455.
class HTTP_API WebSocketFramer
{
public:
    /// Creates a WebSocketFramer operating in the given endpoint mode.
    /// Client-side framers mask outgoing payloads; server-side framers do not.
    /// @param mode ServerSide or ClientSide.
    WebSocketFramer(ws::Mode mode);

    virtual ~WebSocketFramer();

    /// Encodes `data` into a WebSocket frame and writes it to `frame`.
    /// @param data Pointer to the payload data.
    /// @param len Payload length in bytes.
    /// @param flags Frame flags: ws::SendFlags::Text, ws::SendFlags::Binary,
    ///              or a control frame opcode combined with FrameFlags::Fin.
    /// @param frame BitWriter to write the encoded frame into.
    /// @return Total number of bytes written to the frame buffer (header + payload).
    virtual size_t writeFrame(const char* data, size_t len, int flags, BitWriter& frame);

    /// Decodes a single WebSocket frame from `frame`.
    ///
    /// The payload is unmasked in-place in the source buffer; no copy is made.
    /// `payload` is set to point at the start of the payload within `frame`'s buffer.
    ///
    /// @param frame BitReader positioned at the start of a frame.
    /// @param payload Set to point at the start of the decoded payload. Not null-terminated.
    /// @return Payload length in bytes.
    /// @throws std::runtime_error on protocol violations or if the buffer is too small.
    virtual uint64_t readFrame(BitReader& frame, char*& payload);

    /// Returns true if the WebSocket handshake has completed successfully.
    [[nodiscard]] bool handshakeComplete() const;

    //
    /// Server side

    /// Validates the client upgrade request and writes a 101 Switching Protocols response.
    /// Sets the internal state to mark the handshake as complete.
    /// @param request Incoming HTTP upgrade request.
    /// @param response HTTP response to populate with the handshake reply.
    /// @throws std::runtime_error if the request is not a valid WebSocket upgrade.
    void acceptServerRequest(http::Request& request, http::Response& response);

    //
    /// Client side

    /// Populates `request` with the WebSocket upgrade headers (Connection, Upgrade,
    /// Sec-WebSocket-Key, Sec-WebSocket-Version) to initiate the handshake.
    /// @param request HTTP request to add upgrade headers to.
    void createClientHandshakeRequest(http::Request& request);

    /// Validates the server's 101 Switching Protocols response.
    /// @param response The HTTP response received from the server.
    /// @return true if the handshake succeeded and data can flow.
    /// @throws std::runtime_error if the server rejected or mishandled the upgrade.
    bool checkClientHandshakeResponse(http::Response& response);

    /// Completes the client-side handshake by verifying Connection, Upgrade and
    /// Sec-WebSocket-Accept headers. Advances internal state to "complete".
    /// @param response The 101 Switching Protocols response from the server.
    /// @throws std::runtime_error if any required header is missing or incorrect.
    void completeClientHandshake(http::Response& response);

protected:
    /// Returns the frame flags of the most recently received frame.
    /// Set by readFrame()
    [[nodiscard]] int frameFlags() const;

    /// Returns true if the payload must be masked.
    /// Used by writeFrame()
    [[nodiscard]] bool mustMaskPayload() const;

    [[nodiscard]] ws::Mode mode() const;

    enum
    {
        FRAME_FLAG_MASK = 0x80,
        MAX_HEADER_LENGTH = 14,
        MAX_MESSAGE_SIZE = 64 * 1024 * 1024 // 64 MB max reassembled message
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
    /// Creates a WebSocketAdapter using the given socket, mode and HTTP message objects.
    /// @param socket The underlying TCP or SSL socket.
    /// @param mode ServerSide or ClientSide.
    /// @param request HTTP request used for the handshake.
    /// @param response HTTP response used for the handshake.
    WebSocketAdapter(const net::Socket::Ptr& socket, ws::Mode mode,
                     http::Request& request, http::Response& response);

    /// Frames and sends data to the peer's address.
    /// @param data Pointer to the payload.
    /// @param len Payload length in bytes.
    /// @param flags ws::SendFlags::Text or ws::SendFlags::Binary.
    /// @return Number of bytes sent, or -1 on error.
    virtual ssize_t send(const char* data, size_t len, int flags = 0) override;                               // flags = ws::Text || ws::Binary

    /// Frames and sends data to a specific peer address (for UDP-backed sockets).
    /// @param data Pointer to the payload.
    /// @param len Payload length in bytes.
    /// @param peerAddr Destination address.
    /// @param flags ws::SendFlags::Text or ws::SendFlags::Binary.
    /// @return Number of bytes sent, or -1 on error.
    virtual ssize_t send(const char* data, size_t len, const net::Address& peerAddr, int flags = 0) override; // flags = ws::Text || ws::Binary
    virtual ssize_t sendOwned(Buffer&& buffer, int flags = 0) override;
    virtual ssize_t sendOwned(Buffer&& buffer, const net::Address& peerAddr, int flags = 0) override;

    /// Sends a WebSocket CLOSE frame with the given status code and message,
    /// then closes the underlying socket.
    /// @param statusCode WebSocket close status code (e.g. 1000 for normal close).
    /// @param statusMessage Human-readable reason for closing.
    /// @return true if the close frame was sent successfully.
    virtual bool shutdown(uint16_t statusCode, const std::string& statusMessage);

    /// Pointer to the underlying socket.
    /// Sent data will be proxied to this socket.
    net::Socket::Ptr socket;

    //
    /// Client side

    /// Sends the WebSocket HTTP upgrade request to initiate the handshake.
    /// Called automatically on socket connect.
    virtual void sendClientRequest();

    /// Parses the server's HTTP upgrade response and completes the handshake.
    /// Any data remaining in the buffer after the HTTP response is re-fed as WebSocket frames.
    /// @param buffer Buffer containing the server's HTTP response.
    /// @param peerAddr Address of the peer.
    virtual void handleClientResponse(const MutableBuffer& buffer, const net::Address& peerAddr);

    //
    /// Server side

    /// Parses the client's HTTP upgrade request and sends the 101 response.
    /// @param buffer Buffer containing the client's HTTP upgrade request.
    /// @param peerAddr Address of the peer.
    virtual void handleServerRequest(const MutableBuffer& buffer, const net::Address& peerAddr);

    /// @private Called by the socket on connect; initiates the client handshake.
    virtual bool onSocketConnect(net::Socket& socket) override;

    /// @private Called by the socket on each received buffer;
    /// handles handshake or frame parsing depending on state.
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress) override;

    /// @private Called by the socket on close; resets framer state.
    virtual bool onSocketClose(net::Socket& socket) override;

    /// Called when the WebSocket handshake completes.
    /// Emits the connect event to downstream handlers.
    virtual void onHandshakeComplete();

protected:
    virtual ~WebSocketAdapter();

    bool sendControlFrame(ws::Opcode opcode,
                          const char* payload,
                          size_t payloadLen,
                          const net::Address& peerAddr);
    void resetFrameState();

    friend class WebSocketFramer;

    WebSocketFramer framer;
    http::Request& _request;
    http::Response& _response;
    ws::CloseState _closeState{ws::CloseState::Open};
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

    /// Returns the HTTP request used during the WebSocket handshake.
    [[nodiscard]] http::Request& request();

    /// Returns the HTTP response received during the WebSocket handshake.
    [[nodiscard]] http::Response& response();

protected:
    http::Request _request;
    http::Response _response;
};


//
// WebSocket Connection Adapter
//


/// WebSocket class which belongs to an HTTP connection.
class HTTP_API ConnectionAdapter : public WebSocketAdapter
{
public:
    /// Creates a ConnectionAdapter for upgrading an existing HTTP connection to WebSocket.
    /// Disables automatic header sending on the underlying connection.
    /// @param connection The HTTP connection to upgrade.
    /// @param mode ServerSide or ClientSide.
    ConnectionAdapter(Connection* connection, ws::Mode mode);
    virtual ~ConnectionAdapter();

    /// Called when the WebSocket handshake completes.
    /// Emits the connect event via the socket emitter chain.
    virtual void onHandshakeComplete();

protected:
    Connection* _connection;
};


} // namespace ws
} // namespace http
} // namespace icy


/// @}
