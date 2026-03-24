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
#include <cstring>
#include <inttypes.h>
#include <limits>
#include <stdexcept>
#include <string_view>


namespace icy {
namespace http {
namespace ws {


namespace {

bool headerContainsToken(std::string_view header, std::string_view token)
{
    for (size_t pos = 0; pos < header.size(); ) {
        size_t end = header.find(',', pos);
        if (end == std::string_view::npos)
            end = header.size();

        std::string_view part(header.data() + pos, end - pos);
        while (!part.empty() && (part.front() == ' ' || part.front() == '\t'))
            part.remove_prefix(1);
        while (!part.empty() && (part.back() == ' ' || part.back() == '\t'))
            part.remove_suffix(1);

        if (util::icompare(part, token) == 0)
            return true;

        pos = end + 1;
    }
    return false;
}

/// XOR mask/unmask a WebSocket payload using 64-bit word operations.
/// Uses memcpy for type-punning to avoid strict aliasing violations
/// and unaligned access UB. The compiler optimizes memcpy of fixed
/// sizes into single load/store instructions.
inline void applyMask(uint8_t* data, size_t len, uint32_t mask)
{
    uint64_t mask64 = uint64_t(mask) | (uint64_t(mask) << 32);
    size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        uint64_t word;
        std::memcpy(&word, data + i, 8);
        word ^= mask64;
        std::memcpy(data + i, &word, 8);
    }
    auto* m = reinterpret_cast<const uint8_t*>(&mask);
    for (; i < len; i++) {
        data[i] ^= m[i & 3];
    }
}

void reserveExponential(Buffer& buffer, size_t required, size_t floorCapacity = 0)
{
    if (required <= buffer.capacity())
        return;

    size_t newCapacity = buffer.capacity();
    if (newCapacity == 0)
        newCapacity = floorCapacity > 0 ? floorCapacity : required;
    while (newCapacity < required) {
        if (newCapacity > (std::numeric_limits<size_t>::max() / 2)) {
            newCapacity = required;
            break;
        }
        newCapacity *= 2;
    }
    buffer.reserve(newCapacity);
}

[[noreturn]] void throwWsError(ws::ErrorCode code, std::string message, uint16_t closeStatus = 0)
{
    throw ws::WebSocketException(code, std::move(message), closeStatus);
}

const char* closeReasonFor(uint16_t closeStatus)
{
    switch (closeStatus) {
        case uint16_t(ws::CloseStatusCode::PayloadTooBig):
            return "Message Too Large";
        case uint16_t(ws::CloseStatusCode::ProtocolError):
            return "Protocol Error";
        default:
            return "WebSocket Error";
    }
}

} // namespace


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
    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::shutdown: no socket");
    }
    if (_closeState == ws::CloseState::Closed)
        return false;
    if (_closeState == ws::CloseState::CloseSent)
        return true;

    // Build the Close frame payload: 2-byte status code + reason string.
    // RFC 6455 Section 5.5: control frame payloads must be <= 125 bytes.
    size_t reasonLen = std::min(statusMessage.size(), size_t(123));
    char payload[125];
    BitWriter payloadWriter(payload, sizeof(payload));
    payloadWriter.putU16(statusCode);
    payloadWriter.put(statusMessage.c_str(), reasonLen);
    bool sent = sendControlFrame(ws::Opcode::Close,
                                 payload,
                                 payloadWriter.position(),
                                 socket->peerAddress());
    if (sent)
        _closeState = ws::CloseState::CloseSent;
    return sent;
}


ssize_t WebSocketAdapter::send(const char* data, size_t len, int flags)
{
    return send(data, len, socket->peerAddress(), flags);
}


ssize_t WebSocketAdapter::sendOwned(Buffer&& buffer, int flags)
{
    return sendOwned(std::move(buffer), socket->peerAddress(), flags);
}


ssize_t WebSocketAdapter::send(const char* data, size_t len, const net::Address& peerAddr, int flags)
{
    LTrace("Send: ", len, " bytes");
    if (!framer.handshakeComplete()) {
        throw std::runtime_error("WebSocketAdapter::send: handshake not complete");
    }

    // Set default text flag if none specified
    if (!flags)
        flags = ws::SendFlags::Text;

    // Frame and send the data. Must use heap buffer because libuv
    // write is async — the buffer must outlive this stack frame.
    size_t frameSize = len + WebSocketFramer::MAX_HEADER_LENGTH;
    Buffer heapBuf;
    heapBuf.reserve(frameSize);
    DynamicBitWriter writer(heapBuf);
    framer.writeFrame(data, len, flags, writer);
    if (!socket)
        throw std::runtime_error("WebSocketAdapter::send: no socket");
    return SocketAdapter::sendOwned(std::move(heapBuf), peerAddr, 0);
}


ssize_t WebSocketAdapter::sendOwned(Buffer&& buffer, const net::Address& peerAddr, int flags)
{
    if (!framer.handshakeComplete()) {
        throw std::runtime_error("WebSocketAdapter::sendOwned: handshake not complete");
    }

    if (!flags)
        flags = ws::SendFlags::Text;

    Buffer frameBuf;
    frameBuf.reserve(buffer.size() + WebSocketFramer::MAX_HEADER_LENGTH);
    DynamicBitWriter writer(frameBuf);
    framer.writeFrame(buffer.data(), buffer.size(), flags, writer);
    if (!socket)
        throw std::runtime_error("WebSocketAdapter::sendOwned: no socket");
    return SocketAdapter::sendOwned(std::move(frameBuf), peerAddr, 0);
}


void WebSocketAdapter::sendClientRequest()
{
    framer.createClientHandshakeRequest(_request);

    Buffer requestData;
    requestData.reserve(256);
    _request.write(requestData);
    LTrace("Client request: ", std::string(requestData.begin(), requestData.end()));

    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::sendClientRequest: no socket");
    }
    (void)SocketAdapter::sendOwned(std::move(requestData));
}


bool WebSocketAdapter::sendControlFrame(ws::Opcode opcode,
                                        const char* payload,
                                        size_t payloadLen,
                                        const net::Address& peerAddr)
{
    Buffer frameBuf;
    frameBuf.reserve(payloadLen + WebSocketFramer::MAX_HEADER_LENGTH);
    DynamicBitWriter writer(frameBuf);
    int flags = unsigned(ws::FrameFlags::Fin) | unsigned(opcode);
    framer.writeFrame(payload ? payload : "", payloadLen, flags, writer);
    return SocketAdapter::sendOwned(std::move(frameBuf), peerAddr, 0) > 0;
}


void WebSocketAdapter::resetFrameState()
{
    _request.clear();
    _response.clear();
    framer._headerState = 0;
    framer._frameFlags = 0;
    framer._fragmented = false;
    framer._fragmentOpcode = 0;
    framer._fragmentBuffer.clear();
    framer._incompleteFrame.clear();
    _closeState = ws::CloseState::Closed;
}


void WebSocketAdapter::handleClientResponse(const MutableBuffer& buffer, const net::Address& peerAddr)
{
    LTrace("Client response: ", buffer.str());

    auto data = bufferCast<char*>(buffer);
    http::Parser parser(&_response);
    auto result = parser.parse(data, buffer.size());
    if (!result.ok()) {
        throw std::runtime_error(
            "WebSocket error: Cannot parse response: " + result.error.message);
    }
    if (!result.messageComplete && !result.upgrade) {
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
    size_t remaining = buffer.size() - result.bytesConsumed;
    if (remaining) {
        onSocketRecv(*socket.get(), MutableBuffer(&data[result.bytesConsumed], remaining), peerAddr);
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
    auto result = parser.parse(bufferCast<char*>(buffer), buffer.size());
    if (!result.ok()) {
        throw std::runtime_error("WebSocket error: Cannot parse request: " + result.error.message);
    }
    if (!result.upgrade && !result.messageComplete) {
        throw std::runtime_error("WebSocket error: Cannot parse request: Incomplete HTTP message");
    }

    LTrace("Verifying handshake: ", _request);

    // Verify the WebSocket handshake request
    framer.acceptServerRequest(_request, _response);
    LTrace("Handshake success");

    // Send the 101 Switching Protocols response
    Buffer responseData;
    responseData.reserve(256);
    _response.write(responseData);

    if (!socket) {
        throw std::runtime_error("WebSocketAdapter::handleServerRequest: no socket");
    }
    (void)SocketAdapter::sendOwned(std::move(responseData));

    if (result.bytesConsumed < buffer.size()) {
        auto* raw = bufferCast<char*>(buffer);
        onSocketRecv(*socket.get(),
                     MutableBuffer(raw + result.bytesConsumed,
                                   buffer.size() - result.bytesConsumed),
                     peerAddr);
    }

    // Note: onHandshakeComplete() is NOT called here because on the
    // server side this runs inside onHeaders() before onConnectionReady
    // or createResponder. The server fires onHandshakeComplete() from
    // ServerConnection::onHeaders() after the responder is created so
    // that application handlers are attached before the connect signal.
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

    if (!socket || socket->closed() || _closeState == ws::CloseState::Closed ||
        _closeState == ws::CloseState::CloseReceived) {
        return false;
    }

    if (framer.handshakeComplete()) {

        // If we have buffered incomplete frame data from a previous recv,
        // prepend it to the current buffer.
        Buffer workBuf;
        const char* data;
        size_t dataLen;
        if (!framer._incompleteFrame.empty()) {
            workBuf.reserve(framer._incompleteFrame.size() + buffer.size());
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
                    (void)sendControlFrame(ws::Opcode::Pong,
                                           payload,
                                           static_cast<size_t>(payloadLength),
                                           peerAddress);
                    continue;
                }

                if (opcode == unsigned(ws::Opcode::Close)) {
                    LTrace("Received Close frame");
                    if (_closeState == ws::CloseState::Open) {
                        (void)sendControlFrame(ws::Opcode::Close,
                                               payload,
                                               static_cast<size_t>(payloadLength),
                                               peerAddress);
                    }
                    _closeState = ws::CloseState::CloseReceived;
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
                        throwWsError(ws::ErrorCode::ProtocolViolation,
                            "WebSocket error: Unexpected continuation frame (protocol error 1002)",
                            uint16_t(ws::CloseStatusCode::ProtocolError));
                    }
                    if (payload && payloadLength > 0) {
                        if (framer._fragmentBuffer.size() + payloadLength > WebSocketFramer::MAX_MESSAGE_SIZE) {
                            throwWsError(ws::ErrorCode::PayloadTooBig,
                                "WebSocket error: Message too large (close 1009)",
                                uint16_t(ws::CloseStatusCode::PayloadTooBig));
                        }
                        reserveExponential(
                            framer._fragmentBuffer,
                            framer._fragmentBuffer.size() + static_cast<size_t>(payloadLength),
                            4096);
                        framer._fragmentBuffer.insert(framer._fragmentBuffer.end(),
                            payload, payload + static_cast<size_t>(payloadLength));
                    }
                    if (isFin) {
                        // Final fragment - emit the complete reassembled message
                        framer._fragmented = false;
                        if (!framer._fragmentBuffer.empty()) {
                            net::SocketEmitter::onSocketRecv(*socket.get(),
                                mutableBuffer(framer._fragmentBuffer.data(), framer._fragmentBuffer.size()),
                                peerAddress);
                            if (!socket || socket->closed() ||
                                _closeState != ws::CloseState::Open) {
                                return false;
                            }
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
                        throwWsError(ws::ErrorCode::ProtocolViolation,
                            "WebSocket error: New data frame during fragmentation (protocol error 1002)",
                            uint16_t(ws::CloseStatusCode::ProtocolError));
                    }
                    framer._fragmented = true;
                    framer._fragmentOpcode = opcode;
                    framer._fragmentBuffer.clear();
                    if (payload && payloadLength > 0) {
                        reserveExponential(
                            framer._fragmentBuffer,
                            static_cast<size_t>(payloadLength),
                            std::min<size_t>(
                                std::max<size_t>(static_cast<size_t>(payloadLength) * 2, 4096),
                                WebSocketFramer::MAX_MESSAGE_SIZE));
                        framer._fragmentBuffer.insert(framer._fragmentBuffer.end(),
                            payload, payload + static_cast<size_t>(payloadLength));
                    }
                    continue;
                }

                // Complete single-frame message (FIN set, not continuation)
                if (!payloadLength) {
                    LDebug("Dropping empty frame");
                    continue;
                }

            } catch (const ws::WebSocketException& exc) {
                if (exc.code() == ws::ErrorCode::IncompleteFrame) {
                    size_t remaining = total - posBeforeRead;
                    framer._incompleteFrame.clear();
                    framer._incompleteFrame.reserve(remaining);
                    framer._incompleteFrame.insert(
                        framer._incompleteFrame.end(),
                        data + posBeforeRead, data + posBeforeRead + remaining);
                    LTrace("Buffering incomplete frame: ", remaining, " bytes");
                    return false;
                }

                LError("Parser error: ", exc.what());
                if (exc.hasCloseStatus()) {
                    try {
                        shutdown(exc.closeStatus(), closeReasonFor(exc.closeStatus()));
                    } catch (...) {
                    }
                }
                socket->setError(exc.what());
                return false;
            } catch (std::exception& exc) {
                LError("Parser error: ", exc.what());
                socket->setError(exc.what());
                return false;
            }

            // Emit complete single-frame data packet
            if (payload && payloadLength) {
                net::SocketEmitter::onSocketRecv(*socket.get(),
                    mutableBuffer(payload, static_cast<size_t>(payloadLength)),
                    peerAddress);
                if (!socket || socket->closed() ||
                    _closeState != ws::CloseState::Open) {
                    return false;
                }
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
            socket->close();
        }
        return false;
    }
    return false;
}


bool WebSocketAdapter::onSocketClose(net::Socket&)
{
    LTrace("On close");

    _closeState = ws::CloseState::Closed;

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
    _connection->setHeaderAutoSendEnabled(false);
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

    if (headerContainsToken(request.get("Connection", ""), "Upgrade") &&
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
    // Validate opcode: data frames (Text, Binary, Continuation) and control frames
    int opcode = flags & unsigned(ws::Opcode::Bitmask);
    if (opcode != unsigned(ws::Opcode::Continuation) &&
        opcode != unsigned(ws::Opcode::Text) &&
        opcode != unsigned(ws::Opcode::Binary) &&
        opcode != unsigned(ws::Opcode::Close) &&
        opcode != unsigned(ws::Opcode::Ping) &&
        opcode != unsigned(ws::Opcode::Pong)) {
        throw std::runtime_error("WebSocketFramer::writeFrame: invalid opcode");
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
        uint32_t mask = _rnd.next();
        frame.put(reinterpret_cast<const char*>(&mask), 4);

        // Write payload then XOR mask in-place
        size_t payloadPos = frame.position();
        frame.put(data, len);
        applyMask(reinterpret_cast<uint8_t*>(frame.begin() + payloadPos), len, mask);
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
    size_t offset = frame.position();
    size_t available = frame.available();
    if (available < 2) {
        throwWsError(ws::ErrorCode::IncompleteFrame,
            "WebSocket error: Incomplete frame received");
    }

    const char* frameData = frame.begin() + offset;
    uint8_t lengthByte = static_cast<uint8_t>(frameData[1]);
    bool hasMask = (lengthByte & FRAME_FLAG_MASK) != 0;
    uint8_t payloadTag = lengthByte & 0x7f;
    size_t headerLength = 2 + (payloadTag == 126 ? 2 : (payloadTag == 127 ? 8 : 0)) +
        (hasMask ? 4 : 0);
    if (available < headerLength) {
        throwWsError(ws::ErrorCode::IncompleteFrame,
            "WebSocket error: Incomplete frame received");
    }

    BitReader headerReader(mutableBuffer(const_cast<char*>(frameData), headerLength));
    uint8_t flags = 0;
    char mask[4] = {};
    headerReader.getU8(flags);
    headerReader.getU8(lengthByte);
    _frameFlags = flags;

    // RFC 6455 Section 5.2: RSV1, RSV2, RSV3 MUST be 0 unless an extension
    // is negotiated that defines meanings for non-zero values.
    if (flags & (unsigned(ws::FrameFlags::Rsv1) | unsigned(ws::FrameFlags::Rsv2) | unsigned(ws::FrameFlags::Rsv3))) {
        throwWsError(ws::ErrorCode::InvalidRsvBits,
            "WebSocket error: RSV bits set without extension negotiation (protocol error 1002)",
            uint16_t(ws::CloseStatusCode::ProtocolError));
    }

    // RFC 6455 Section 5.2: Validate opcode - reject reserved/unknown opcodes
    unsigned opcode = flags & unsigned(ws::Opcode::Bitmask);
    if (opcode > unsigned(ws::Opcode::Binary) && opcode < unsigned(ws::Opcode::Close)) {
        throwWsError(ws::ErrorCode::InvalidOpcode,
            "WebSocket error: Reserved opcode received (protocol error 1002)",
            uint16_t(ws::CloseStatusCode::ProtocolError));
    }
    if (opcode > unsigned(ws::Opcode::Pong)) {
        throwWsError(ws::ErrorCode::InvalidOpcode,
            "WebSocket error: Unknown opcode received (protocol error 1002)",
            uint16_t(ws::CloseStatusCode::ProtocolError));
    }

    // RFC 6455 Section 5.5: Control frames MUST have FIN set and payload <= 125 bytes
    bool isControl = (opcode >= unsigned(ws::Opcode::Close));
    if (isControl) {
        if (!(flags & unsigned(ws::FrameFlags::Fin))) {
            throwWsError(ws::ErrorCode::ProtocolViolation,
                "WebSocket error: Fragmented control frame (protocol error 1002)",
                uint16_t(ws::CloseStatusCode::ProtocolError));
        }
        if ((lengthByte & 0x7f) > 125) {
            throwWsError(ws::ErrorCode::ProtocolViolation,
                "WebSocket error: Control frame payload too large (protocol error 1002)",
                uint16_t(ws::CloseStatusCode::ProtocolError));
        }
    }

    // RFC 6455 Section 5.1: Server MUST reject unmasked client-to-server frames
    if (_mode == ws::ServerSide && !hasMask) {
        throwWsError(ws::ErrorCode::UnmaskedClientFrame,
            "WebSocket error: Client frame not masked (protocol error 1002)",
            uint16_t(ws::CloseStatusCode::ProtocolError));
    }

    uint64_t payloadLength = 0;
    if ((lengthByte & 0x7f) == 127) {
        uint64_t l;
        headerReader.getU64(l);
        payloadLength = l;
    } else if ((lengthByte & 0x7f) == 126) {
        uint16_t l;
        headerReader.getU16(l);
        payloadLength = l;
    } else {
        payloadLength = lengthByte & 0x7f;
    }
    if (hasMask) {
        headerReader.get(mask, 4);
    }

    // Check that header + payload fit within the available buffer
    if (payloadLength > (available - headerLength)) {
        throwWsError(ws::ErrorCode::IncompleteFrame,
            "WebSocket error: Incomplete frame received");
    }

    // Get a reference to the start of the payload
    payload = const_cast<char*>(frameData + headerLength);

    // Unmask the payload in-place using 64-bit words
    if (hasMask) {
        uint32_t mask32;
        std::memcpy(&mask32, mask, 4);
        applyMask(reinterpret_cast<uint8_t*>(payload),
                  static_cast<size_t>(payloadLength), mask32);
    }

    // Update frame length to include payload plus header
    frame.seek(static_cast<size_t>(offset + headerLength + payloadLength));

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

    if (!headerContainsToken(response.get("Connection", ""), "Upgrade"))
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
