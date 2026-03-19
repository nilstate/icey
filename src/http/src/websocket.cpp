///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/websocket.h"
#include "icy/base64.h"
#include "icy/crypto/hash.h"
#include "icy/http/client.h"
#include "icy/http/server.h"
#include "icy/logger.h"
#include "icy/numeric.h"
#include "icy/random.h"
#include <inttypes.h>
#include <stdexcept>


namespace icy {
namespace http {
namespace ws {


WebSocket::WebSocket(const net::Socket::Ptr& socket)
    : WebSocketAdapter(socket, ws::ClientSide, _request, _response)
{
}


WebSocket::~WebSocket()
{
}


http::Request& WebSocket::request()
{
    return _request;
}


http::Response& WebSocket::response()
{
    return _response;
}


//
// WebSocket Adapter
//


WebSocketAdapter::WebSocketAdapter(const net::Socket::Ptr& socket,
                                   ws::Mode mode, http::Request& request,
                                   http::Response& response)
    : SocketEmitter(socket)
    , socket(socket)
    , framer(mode)
    , _request(request)
    , _response(response)
{
    LTrace("Create");
}


WebSocketAdapter::~WebSocketAdapter()
{
    LTrace("Destroy");
}


bool WebSocketAdapter::shutdown(uint16_t statusCode, const std::string& statusMessage)
{
    char buffer[256];
    BitWriter writer(buffer, 256);
    writer.putU16(statusCode);
    writer.put(statusMessage);

    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::shutdown: no socket");
    }
    return SocketAdapter::send(buffer, writer.position(),
                               unsigned(ws::FrameFlags::Fin) | unsigned(ws::Opcode::Close)) > 0;
}


ssize_t WebSocketAdapter::send(const char* data, size_t len, int flags)
{
    return send(data, len, socket->peerAddress(), flags);
}


ssize_t WebSocketAdapter::send(const char* data, size_t len, const net::Address& peerAddr, int flags)
{
    LTrace("Send: ", len, ": ", std::string(data, len));
    if (!framer.handshakeComplete()) {
        throw std::runtime_error("WebSocketAdapter::send: handshake not complete");
    }

    // Set default text flag if none specified
    if (!flags)
        flags = ws::SendFlags::Text;

    // Frame and send the data
    Buffer buffer;
    buffer.reserve(len + WebSocketFramer::MAX_HEADER_LENGTH);
    BitWriter writer(buffer);
    framer.writeFrame(data, len, flags, writer);

    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::send: no socket");
    }
    return SocketAdapter::send(writer.begin(), writer.position(), peerAddr, 0);
}


void WebSocketAdapter::sendClientRequest()
{
    framer.createClientHandshakeRequest(_request);

    std::ostringstream oss;
    _request.write(oss);
    LTrace("Client request: ", oss.str());

    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::sendClientRequest: no socket");
    }
    (void)SocketAdapter::send(oss.str().c_str(), oss.str().length());
}


void WebSocketAdapter::handleClientResponse(const MutableBuffer& buffer, const net::Address& peerAddr)
{
    LTrace("Client response: ", buffer.str());

    auto data = bufferCast<char*>(buffer);
    http::Parser parser(&_response);
    size_t nparsed = parser.parse(data, buffer.size());
    if (nparsed == 0) {
        throw std::runtime_error(
            "WebSocket error: Cannot parse response: Incomplete HTTP message");
    }

    // Parse and check the response
    if (framer.checkClientHandshakeResponse(_response)) {
        LTrace("Handshake success");
        onHandshakeComplete();
    }

    // If there is remaining data in the packet (packets may be joined)
    // then send it back through the socket recv method.
    size_t remaining = buffer.size() - nparsed;
    if (remaining) {
        onSocketRecv(*socket.get(), MutableBuffer(&data[nparsed], remaining), peerAddr);
    }
}


void WebSocketAdapter::onHandshakeComplete()
{
    // Call net::SocketEmitter::onSocketConnect to notify handlers that data may flow
    net::SocketEmitter::onSocketConnect(*socket.get());
}


void WebSocketAdapter::handleServerRequest(const MutableBuffer& buffer, const net::Address& peerAddr)
{
    LTrace("Server request: ", buffer.str());

    http::Parser parser(&_request);
    if (!parser.parse(bufferCast<char*>(buffer), buffer.size())) {
        throw std::runtime_error("WebSocket error: Cannot parse request: Incomplete HTTP message");
    }

    LTrace("Verifying handshake: ", _request);

    // Verify the WebSocket handshake request
    try {
        framer.acceptServerRequest(_request, _response);
        LTrace("Handshake success");
    } catch (std::exception& exc) {
        LWarn("Handshake failed: ", exc.what());
    }

    // Send response
    std::ostringstream oss;
    _response.write(oss);

    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::handleServerRequest: no socket");
    }
    (void)SocketAdapter::send(oss.str().c_str(), oss.str().length());
}


bool WebSocketAdapter::onSocketConnect(net::Socket&)
{
    LTrace("On connect");

    // Send the WS handshake request
    // The Connect signal will be sent after the
    // handshake is complete
    sendClientRequest();
    return false;
}


bool WebSocketAdapter::onSocketRecv(net::Socket&, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    LTrace("On recv: ", buffer.size());

    if (framer.handshakeComplete()) {

        // If we have buffered incomplete frame data from a previous recv,
        // prepend it to the current buffer.
        Buffer workBuf;
        const char* data;
        size_t dataLen;
        if (!framer._incompleteFrame.empty()) {
            workBuf.insert(workBuf.end(),
                           framer._incompleteFrame.begin(), framer._incompleteFrame.end());
            workBuf.insert(workBuf.end(),
                           bufferCast<const char*>(buffer),
                           bufferCast<const char*>(buffer) + buffer.size());
            framer._incompleteFrame.clear();
            data = workBuf.data();
            dataLen = workBuf.size();
        } else {
            data = bufferCast<const char*>(buffer);
            dataLen = buffer.size();
        }

        // Parse frames in a loop - incoming data may contain multiple
        // joined frames, or partial frames that need buffering.
        BitReader reader(mutableBuffer(const_cast<char*>(data), dataLen));
        size_t total = reader.available();
        size_t offset = reader.position();
        while (offset < total) {
            char* payload = nullptr;
            uint64_t payloadLength = 0;
            size_t posBeforeRead = reader.position();
            try {
                // Parse a frame (throws on protocol errors)
                payloadLength = framer.readFrame(reader, payload);

                // Update the next frame offset
                offset = reader.position();
                if (offset < total)
                    LTrace("Splitting joined packet at ", offset, " of ", total);

                // Handle control frames per RFC 6455
                // Control frames (Ping, Pong, Close) may be interleaved
                // between fragmented data frames.
                int opcode = framer.frameFlags() & unsigned(ws::Opcode::Bitmask);
                bool isFin = (framer.frameFlags() & unsigned(ws::FrameFlags::Fin)) != 0;

                if (opcode == unsigned(ws::Opcode::Ping)) {
                    LTrace("Received Ping, sending Pong");
                    int pongFlags = unsigned(ws::FrameFlags::Fin) | unsigned(ws::Opcode::Pong);
                    Buffer pongBuf;
                    pongBuf.reserve(static_cast<size_t>(payloadLength) + WebSocketFramer::MAX_HEADER_LENGTH);
                    BitWriter pongWriter(pongBuf);
                    framer.writeFrame(payload ? payload : "", static_cast<size_t>(payloadLength), pongFlags, pongWriter);
                    (void)SocketAdapter::send(pongWriter.begin(), pongWriter.position(), peerAddress, 0);
                    continue;
                }

                if (opcode == unsigned(ws::Opcode::Close)) {
                    LTrace("Received Close frame, echoing close");
                    int closeFlags = unsigned(ws::FrameFlags::Fin) | unsigned(ws::Opcode::Close);
                    Buffer closeBuf;
                    closeBuf.reserve(static_cast<size_t>(payloadLength) + WebSocketFramer::MAX_HEADER_LENGTH);
                    BitWriter closeWriter(closeBuf);
                    framer.writeFrame(payload ? payload : "", static_cast<size_t>(payloadLength), closeFlags, closeWriter);
                    (void)SocketAdapter::send(closeWriter.begin(), closeWriter.position(), peerAddress, 0);
                    socket->close();
                    return false;
                }

                if (opcode == unsigned(ws::Opcode::Pong)) {
                    LTrace("Received Pong frame, ignoring");
                    continue;
                }

                // RFC 6455 Section 5.4: Handle message fragmentation
                if (opcode == unsigned(ws::Opcode::Continuation)) {
                    // Continuation frame - must be part of a fragmented message
                    if (!framer._fragmented) {
                        throw std::runtime_error("WebSocket error: Unexpected continuation frame (protocol error 1002)");
                    }
                    if (payload && payloadLength > 0)
                        framer._fragmentBuffer.insert(framer._fragmentBuffer.end(),
                            payload, payload + static_cast<size_t>(payloadLength));
                    if (isFin) {
                        // Final fragment - emit the complete reassembled message
                        framer._fragmented = false;
                        if (!framer._fragmentBuffer.empty()) {
                            net::SocketEmitter::onSocketRecv(*socket.get(),
                                mutableBuffer(framer._fragmentBuffer.data(), framer._fragmentBuffer.size()),
                                peerAddress);
                        }
                        framer._fragmentBuffer.clear();
                        framer._fragmentOpcode = 0;
                    }
                    continue;
                }

                // Data frame (Text or Binary)
                if (!isFin) {
                    // First frame of a fragmented message
                    if (framer._fragmented) {
                        throw std::runtime_error("WebSocket error: New data frame during fragmentation (protocol error 1002)");
                    }
                    framer._fragmented = true;
                    framer._fragmentOpcode = opcode;
                    framer._fragmentBuffer.clear();
                    if (payload && payloadLength > 0)
                        framer._fragmentBuffer.insert(framer._fragmentBuffer.end(),
                            payload, payload + static_cast<size_t>(payloadLength));
                    continue;
                }

                // Complete single-frame message (FIN set, not continuation)
                if (!payloadLength) {
                    LDebug("Dropping empty frame");
                    continue;
                }

            } catch (std::exception& exc) {
                // Check if this is an incomplete frame (ran out of data)
                // rather than a protocol error
                std::string msg(exc.what());
                if (msg.find("Insufficient buffer") != std::string::npos ||
                    msg.find("Incomplete frame") != std::string::npos) {
                    // Buffer the remaining data for the next recv call
                    size_t remaining = total - posBeforeRead;
                    framer._incompleteFrame.assign(
                        data + posBeforeRead, data + posBeforeRead + remaining);
                    LTrace("Buffering incomplete frame: ", remaining, " bytes");
                    return false;
                }

                LError("Parser error: ", exc.what());
                if (msg.find("protocol error 1002") != std::string::npos) {
                    try {
                        shutdown(uint16_t(ws::CloseStatusCode::ProtocolError), "Protocol Error");
                    } catch (...) {}
                }
                socket->setError(exc.what());
                return false;
            }

            // Emit complete single-frame data packet
            if (payload && payloadLength) {
                net::SocketEmitter::onSocketRecv(*socket.get(),
                    mutableBuffer(payload, static_cast<size_t>(payloadLength)),
                    peerAddress);
            }
        }
    } else {
        try {
            if (framer.mode() == ws::ClientSide)
                handleClientResponse(buffer, peerAddress);
            else
                handleServerRequest(buffer, peerAddress);
        } catch (std::exception& exc) {
            LError("Read error: ", exc.what());
            socket->setError(exc.what());
        }
        return false;
    }
    return false;
}


bool WebSocketAdapter::onSocketClose(net::Socket&)
{
    LTrace("On close");

    // Reset state so the connection can be reused
    _request.clear();
    _response.clear();
    framer._headerState = 0;
    framer._frameFlags = 0;
    framer._fragmented = false;
    framer._fragmentOpcode = 0;
    framer._fragmentBuffer.clear();
    framer._incompleteFrame.clear();

    // Emit closed event
    return net::SocketEmitter::onSocketClose(*socket.get());
}


//
// WebSocket Connection Adapter
//


ConnectionAdapter::ConnectionAdapter(Connection* connection, ws::Mode mode)
    : WebSocketAdapter(connection->socket(), mode, connection->request(), connection->response())
    , _connection(connection)
{
    // Don't send the default header as the websocket upgrade will
    // be sent via WebSocketAdapter::onSocketConnect()
    _connection->shouldSendHeader(false);
}


ConnectionAdapter::~ConnectionAdapter()
{
}


void ConnectionAdapter::onHandshakeComplete()
{
    net::SocketEmitter::onSocketConnect(*socket.get());
}


//
// WebSocket Framer
//


WebSocketFramer::WebSocketFramer(ws::Mode mode)
    : _mode(mode)
    , _frameFlags(0)
    , _headerState(0)
    , _maskPayload(mode == ws::ClientSide)
{
}


WebSocketFramer::~WebSocketFramer()
{
}


std::string createKey()
{
    return base64::encode(util::randomString(16));
}


std::string computeAccept(const std::string& key)
{
    std::string accept(key);
    crypto::Hash engine("SHA1");
    engine.update(key + ws::ProtocolGuid);
    return base64::encode(engine.digest());
}


void WebSocketFramer::createClientHandshakeRequest(http::Request& request)
{
    if (_mode != ws::ClientSide) {
        throw std::runtime_error("WebSocketFramer: not in client mode");
    }
    if (_headerState != 0) {
        throw std::runtime_error("WebSocketFramer: invalid header state for client handshake");
    }

    // Send the handshake request
    _key = createKey();
    request.setChunkedTransferEncoding(false);
    request.set("Connection", "Upgrade");
    request.set("Upgrade", "websocket");
    request.set("Sec-WebSocket-Version", ws::ProtocolVersion);
    request.set("Sec-WebSocket-Key", _key);
    _headerState++;
}


bool WebSocketFramer::checkClientHandshakeResponse(http::Response& response)
{
    if (_mode != ws::ClientSide) {
        throw std::runtime_error("WebSocketFramer: not in client mode");
    }
    if (_headerState != 1) {
        throw std::runtime_error("WebSocketFramer: invalid header state for response check");
    }

    switch (response.getStatus()) {
        case http::StatusCode::SwitchingProtocols: {

            // Complete handshake or throw
            completeClientHandshake(response);

            // Success
            return true;
        }
        case http::StatusCode::Unauthorized: {
            throw std::runtime_error("WebSocket error: Authentication not implemented");
        }
        default:
            throw std::runtime_error("WebSocket error: Cannot upgrade to WebSocket connection: " + response.getReason());
    }
}


void WebSocketFramer::acceptServerRequest(http::Request& request, http::Response& response)
{
    if (_mode != ws::ServerSide) {
        throw std::runtime_error("WebSocketFramer: not in server mode");
    }

    if ((util::icompare(request.get("Connection", ""), "upgrade") == 0 ||
         util::icompare(request.get("Connection", ""), "keep-alive, Upgrade") == 0) &&
        util::icompare(request.get("Upgrade", ""), "websocket") == 0) {
        std::string version = request.get("Sec-WebSocket-Version", "");
        if (version.empty())
            throw std::runtime_error("WebSocket error: Missing Sec-WebSocket-Version in handshake request");
        if (version != ws::ProtocolVersion)
            throw std::runtime_error("WebSocket error: Unsupported WebSocket version requested: " + version);
        std::string key = util::trim(request.get("Sec-WebSocket-Key", ""));
        if (key.empty())
            throw std::runtime_error("WebSocket error: Missing Sec-WebSocket-Key in handshake request");

        response.setStatus(http::StatusCode::SwitchingProtocols);
        response.set("Upgrade", "websocket");
        response.set("Connection", "Upgrade");
        response.set("Sec-WebSocket-Accept", computeAccept(key));

        // Set headerState 2 since the handshake was accepted.
        _headerState = 2;
    } else
        throw std::runtime_error("WebSocket error: No WebSocket handshake");
}


size_t WebSocketFramer::writeFrame(const char* data, size_t len, int flags, BitWriter& frame)
{
    // Allow data frames (Text, Binary) and control frames (Close, Ping, Pong)
    int opcode = flags & unsigned(ws::Opcode::Bitmask);
    bool isControlFrame = (opcode == unsigned(ws::Opcode::Close) ||
                           opcode == unsigned(ws::Opcode::Ping) ||
                           opcode == unsigned(ws::Opcode::Pong));
    if (flags != ws::SendFlags::Text && flags != ws::SendFlags::Binary && !isControlFrame) {
        throw std::runtime_error("WebSocketFramer::writeFrame: invalid flags");
    }
    if (frame.position() != 0) {
        throw std::runtime_error("WebSocketFramer::writeFrame: frame not at position 0");
    }

    frame.putU8(static_cast<uint8_t>(flags));
    uint8_t lenByte(0);
    if (_maskPayload) {
        lenByte |= FRAME_FLAG_MASK;
    }
    if (len < 126) {
        lenByte |= static_cast<uint8_t>(len);
        frame.putU8(lenByte);
    } else if (len < 65536) {
        lenByte |= 126;
        frame.putU8(lenByte);
        frame.putU16(static_cast<uint16_t>(len));
    } else {
        lenByte |= 127;
        frame.putU8(lenByte);
        frame.putU64(static_cast<uint64_t>(len));
    }

    if (_maskPayload) {
        auto mask = _rnd.next();
        auto m = reinterpret_cast<const char*>(&mask);
        auto b = reinterpret_cast<const char*>(data);
        frame.put(m, 4);
        for (unsigned i = 0; i < len; i++) {
            frame.putU8(b[i] ^ m[i % 4]);
        }
    } else {
        frame.put(data, len);
    }

    return frame.position();
}


uint64_t WebSocketFramer::readFrame(BitReader& frame, char*& payload)
{
    if (!handshakeComplete()) {
        throw std::runtime_error("WebSocketFramer::readFrame: handshake not complete");
    }
    uint64_t limit = frame.limit();
    size_t offset = frame.position();

    // Read the frame header
    char header[MAX_HEADER_LENGTH];
    BitReader headerReader(header, MAX_HEADER_LENGTH);
    frame.get(header, 2);
    uint8_t lengthByte = static_cast<uint8_t>(header[1]);
    int maskOffset = 0;
    if (lengthByte & FRAME_FLAG_MASK)
        maskOffset += 4;
    lengthByte &= 0x7f;
    if (lengthByte + 2 + maskOffset < MAX_HEADER_LENGTH) {
        frame.get(header + 2, lengthByte + maskOffset);
    } else {
        frame.get(header + 2, MAX_HEADER_LENGTH - 2);
    }

    // Reserved fields
    frame.skip(2);

    // Parse frame header
    uint8_t flags;
    char mask[4];
    headerReader.getU8(flags);
    headerReader.getU8(lengthByte);
    _frameFlags = flags;

    // RFC 6455 Section 5.2: RSV1, RSV2, RSV3 MUST be 0 unless an extension
    // is negotiated that defines meanings for non-zero values.
    if (flags & (unsigned(ws::FrameFlags::Rsv1) | unsigned(ws::FrameFlags::Rsv2) | unsigned(ws::FrameFlags::Rsv3))) {
        throw std::runtime_error("WebSocket error: RSV bits set without extension negotiation (protocol error 1002)");
    }

    // RFC 6455 Section 5.2: Validate opcode - reject reserved/unknown opcodes
    unsigned opcode = flags & unsigned(ws::Opcode::Bitmask);
    if (opcode > unsigned(ws::Opcode::Binary) && opcode < unsigned(ws::Opcode::Close)) {
        throw std::runtime_error("WebSocket error: Reserved opcode received (protocol error 1002)");
    }
    if (opcode > unsigned(ws::Opcode::Pong)) {
        throw std::runtime_error("WebSocket error: Unknown opcode received (protocol error 1002)");
    }

    // RFC 6455 Section 5.1: Server MUST reject unmasked client-to-server frames
    bool hasMask = (lengthByte & FRAME_FLAG_MASK) != 0;
    if (_mode == ws::ServerSide && !hasMask) {
        throw std::runtime_error("WebSocket error: Client frame not masked (protocol error 1002)");
    }

    uint64_t payloadLength = 0;
    int payloadOffset = 2;
    if ((lengthByte & 0x7f) == 127) {
        uint64_t l;
        headerReader.getU64(l);
        if (l > limit)
            throw std::runtime_error(
                util::format("WebSocket error: Insufficient buffer for payload size %" PRIu64, l));
        payloadLength = l;
        payloadOffset += 8;
    } else if ((lengthByte & 0x7f) == 126) {
        uint16_t l;
        headerReader.getU16(l);
        if (l > limit)
            throw std::runtime_error(util::format(
                "WebSocket error: Insufficient buffer for payload size %" PRIu64, l));
        payloadLength = l;
        payloadOffset += 2;
    } else {
        uint8_t l = lengthByte & 0x7f;
        if (l > limit)
            throw std::runtime_error(util::format(
                "WebSocket error: Insufficient buffer for payload size %" PRIu64, l));
        payloadLength = l;
    }
    if (lengthByte & FRAME_FLAG_MASK) {
        headerReader.get(mask, 4);
        payloadOffset += 4;
    }

    if (payloadLength > limit)
        throw std::runtime_error(
            "WebSocket error: Incomplete frame received");

    // Get a reference to the start of the payload
    payload = reinterpret_cast<char*>(
        const_cast<char*>(frame.begin() + (offset + payloadOffset)));

    // Unmask the payload if required
    if (lengthByte & FRAME_FLAG_MASK) {
        auto p = reinterpret_cast<char*>(payload);
        for (uint64_t i = 0; i < payloadLength; i++) {
            p[i] ^= mask[i % 4];
        }
    }

    // Update frame length to include payload plus header
    frame.seek(static_cast<size_t>(offset + payloadOffset + payloadLength));

    return payloadLength;
}


void WebSocketFramer::completeClientHandshake(http::Response& response)
{
    if (_mode != ws::ClientSide) {
        throw std::runtime_error("WebSocketFramer: not in client mode");
    }
    if (_headerState != 1) {
        throw std::runtime_error("WebSocketFramer: invalid header state for handshake completion");
    }

    std::string connection = response.get("Connection", "");
    if (util::icompare(connection, "Upgrade") != 0)
        throw std::runtime_error("WebSocket error: No \"Connection: Upgrade\" header in handshake response");
    std::string upgrade = response.get("Upgrade", "");
    if (util::icompare(upgrade, "websocket") != 0)
        throw std::runtime_error("WebSocket error: No \"Upgrade: websocket\" header in handshake response");
    std::string accept = response.get("Sec-WebSocket-Accept", "");
    if (accept != computeAccept(_key))
        throw std::runtime_error("WebSocket error: Invalid or missing Sec-WebSocket-Accept header in handshake response");

    _headerState++;
    if (!handshakeComplete()) {
        throw std::runtime_error("WebSocketFramer: handshake should be complete");
    }
}


ws::Mode WebSocketFramer::mode() const
{
    return _mode;
}


bool WebSocketFramer::handshakeComplete() const
{
    return _headerState == 2;
}


int WebSocketFramer::frameFlags() const
{
    return _frameFlags;
}


bool WebSocketFramer::mustMaskPayload() const
{
    return _maskPayload;
}


} // namespace ws
} // namespace http
} // namespace icy


/// @}
