///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/websocket.h"
#include "scy/base64.h"
#include "scy/crypto/hash.h"
#include "scy/http/client.h"
#include "scy/http/server.h"
#include "scy/logger.h"
#include "scy/numeric.h"
#include "scy/random.h"
#include <stdexcept>
#include <inttypes.h>


using std::endl;


namespace scy {
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
    : SocketEmitter(socket) //.get()
    , socket(socket)
    , framer(mode)
    , _request(request)
    , _response(response)
{
    LTrace("Create")

    // socket->addReceiver(this, 100);
}


WebSocketAdapter::~WebSocketAdapter()
{
    LTrace("Destroy")

    // socket->removeReceiver(this);
}


bool WebSocketAdapter::shutdown(uint16_t statusCode, const std::string& statusMessage)
{
    char buffer[256];
    BitWriter writer(buffer, 256);
    writer.putU16(statusCode);
    writer.put(statusMessage);

    assert(socket);
    return SocketAdapter::send(buffer, writer.position(),
                               unsigned(ws::FrameFlags::Fin) | unsigned(ws::Opcode::Close)) > 0;
}


ssize_t WebSocketAdapter::send(const char* data, size_t len, int flags)
{
    return send(data, len, socket->peerAddress(), flags);
}


ssize_t WebSocketAdapter::send(const char* data, size_t len, const net::Address& peerAddr, int flags)
{
    LTrace("Send: ",  len,  ": ", std::string(data, len))
    assert(framer.handshakeComplete());

    // Set default text flag if none specified
    if (!flags)
        flags = ws::SendFlags::Text;

    // Frame and send the data
    Buffer buffer;
    buffer.reserve(len + WebSocketFramer::MAX_HEADER_LENGTH);
    BitWriter writer(buffer);
    framer.writeFrame(data, len, flags, writer);

    assert(socket);
    return SocketAdapter::send(writer.begin(), writer.position(), peerAddr, 0);
}


void WebSocketAdapter::sendClientRequest()
{
    framer.createClientHandshakeRequest(_request);

    std::ostringstream oss;
    _request.write(oss);
    LTrace("Client request: ", oss.str())

    assert(socket);
    SocketAdapter::send(oss.str().c_str(), oss.str().length());
}


void WebSocketAdapter::handleClientResponse(const MutableBuffer& buffer, const net::Address& peerAddr)
{
    LTrace("Client response: ", buffer.str())

    auto data = bufferCast<char*>(buffer);
    http::Parser parser(&_response);
    size_t nparsed = parser.parse(data, buffer.size());
    if (nparsed == 0) {
        throw std::runtime_error(
            "WebSocket error: Cannot parse response: Incomplete HTTP message");
    }

    // TODO: Handle resending request for authentication
    // Should we implement some king of callback for this?

    // Parse and check the response

    if (framer.checkClientHandshakeResponse(_response)) {
        LTrace("Handshake success")
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
    LTrace("Server request: ", buffer.str())

    http::Parser parser(&_request);
    if (!parser.parse(bufferCast<char*>(buffer), buffer.size())) {
        throw std::runtime_error("WebSocket error: Cannot parse request: Incomplete HTTP message");
    }

    LTrace("Verifying handshake: ", _request)

    // Allow the application to verify the incoming request.
    // TODO: Handle authentication
    // VerifyServerRequest.emit(/*this, */request);

    // Verify the WebSocket handshake request
    try {
        framer.acceptServerRequest(_request, _response);
        LTrace("Handshake success")
    } catch (std::exception& exc) {
        LWarn("Handshake failed: ", exc.what())
    }

    // Allow the application to override the response
    // PrepareServerResponse.emit(/*this, */response);

    // Send response
    std::ostringstream oss;
    _response.write(oss);

    assert(socket);
    SocketAdapter::send(oss.str().c_str(), oss.str().length());
}


void WebSocketAdapter::onSocketConnect(net::Socket&)
{
    LTrace("On connect")


    // Send the WS handshake request
    // The Connect signal will be sent after the
    // handshake is complete
    sendClientRequest();
}


void WebSocketAdapter::onSocketRecv(net::Socket&, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    LTrace("On recv: ", buffer.size())

    if (framer.handshakeComplete()) {

        // Note: The spec wants us to buffer partial frames, but our
        // software does not require this feature, and furthermore
        // it goes against our nocopy where possible policy.
        // This may need to change in the future, but for now
        // we just parse and emit packets as they arrive.
        //
        // Incoming frames may be joined, so we parse them
        // in a loop until the read buffer is empty.
        BitReader reader(buffer);
        size_t total = reader.available();
        size_t offset = reader.position();
        while (offset < total) {
            char* payload = nullptr;
            uint64_t payloadLength = 0;
            try {
                // Restore buffer state for next read
                // reader.position(offset);
                // reader.limit(total);

                // STrace << "Read frame at: "
                //      << "\n\tinputPosition: " << offset
                //      << "\n\tinputLength: " << total
                //      << "\n\tbufferPosition: " << reader.position()
                //      << "\n\tbufferAvailable: " << reader.available()
                //      << "\n\tbufferLimit: " << reader.limit()
                //      << "\n\tbuffer: " << std::string(reader.current(),
                //      reader.limit())
                //      << endl;

                // Parse a frame to throw
                // int payloadLength = framer.readFrame(reader);
                payloadLength = framer.readFrame(reader, payload);
                assert(payload);

                // Update the next frame offset
                offset = reader.position(); // + payloadLength;
                if (offset < total)
                    LTrace("Splitting joined packet at ",  offset,  " of ", total)

                // Drop empty packets
                if (!payloadLength) {
                    LDebug("Dropping empty frame")
                    continue;
                }
            } catch (std::exception& exc) {
                LError("Parser error: ", exc.what())
                socket->setError(exc.what());
                return;
            }

            // Emit the result packet
            assert(payload);
            assert(payloadLength);
            net::SocketEmitter::onSocketRecv(*socket.get(),
                mutableBuffer(payload, (size_t)payloadLength),
                peerAddress);
        }
        assert(offset == total);
    } else {
        try {
            if (framer.mode() == ws::ClientSide)
                handleClientResponse(buffer, peerAddress);
            else
                handleServerRequest(buffer, peerAddress);
        } catch (std::exception& exc) {
            LError("Read error: ", exc.what())
            socket->setError(exc.what());
        }
        return;
    }
}


void WebSocketAdapter::onSocketClose(net::Socket&)
{
    LTrace("On close")

    // Reset state so the connection can be reused
    _request.clear();
    _response.clear();
    framer._headerState = 0;
    framer._frameFlags = 0;

    // Emit closed event
    net::SocketEmitter::onSocketClose(*socket.get());
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
    assert(_mode == ws::ClientSide);
    assert(_headerState == 0);

    // Send the handshake request
    _key = createKey();
    // request.clear();
    request.setChunkedTransferEncoding(false);
    request.set("Connection", "Upgrade");
    request.set("Upgrade", "websocket");
    request.set("Sec-WebSocket-Version", ws::ProtocolVersion);
    assert(request.has("Sec-WebSocket-Version"));
    request.set("Sec-WebSocket-Key", _key);
    assert(request.has("Sec-WebSocket-Key"));
    _headerState++;
}


bool WebSocketFramer::checkClientHandshakeResponse(http::Response& response)
{
    assert(_mode == ws::ClientSide);
    assert(_headerState == 1);

    switch (response.getStatus()) {
        case http::StatusCode::SwitchingProtocols: {

            // Complete handshake or throw
            completeClientHandshake(response);

            // Success
            return true;
        }
        case http::StatusCode::Unauthorized: {
            assert(0 && "authentication not implemented");
            throw std::runtime_error("WebSocket error: Authentication not implemented"); // ws::ErrorNoHandshake
        }
        // case http::StatusCode::UpgradeRequired: {
        //     // The latest node `ws` package always returns a 426 Upgrade
        //     Required
        //     // response, so resend the client websocket updrage request.
        //     _headerState--;
        //
        //     // Need to resend request
        //     return false;
        // }
        default:
            throw std::runtime_error("WebSocket error: Cannot upgrade to WebSocket connection: " + response.getReason()); // ws::ErrorNoHandshake
    }
}


void WebSocketFramer::acceptServerRequest(http::Request& request, http::Response& response)
{
    assert(_mode == ws::ServerSide);

    if ((util::icompare(request.get("Connection", ""), "upgrade") == 0 ||
        util::icompare(request.get("Connection", ""), "keep-alive, Upgrade") == 0) &&
        util::icompare(request.get("Upgrade", ""), "websocket") == 0) {
        std::string version = request.get("Sec-WebSocket-Version", "");
        if (version.empty())
            throw std::runtime_error("WebSocket error: Missing Sec-WebSocket-Version in handshake request"); //, ws::ErrorHandshakeNoVersion
        if (version != ws::ProtocolVersion)
            throw std::runtime_error( "WebSocket error: Unsupported WebSocket version requested: " + version); //, ws::ErrorHandshakeUnsupportedVersion
        std::string key = util::trim(request.get("Sec-WebSocket-Key", ""));
        if (key.empty())
            throw std::runtime_error("WebSocket error: Missing Sec-WebSocket-Key in handshake request"); //, ws::ErrorHandshakeNoKey

        response.setStatus(http::StatusCode::SwitchingProtocols);
        response.set("Upgrade", "websocket");
        response.set("Connection", "Upgrade");
        response.set("Sec-WebSocket-Accept", computeAccept(key));

        // Set headerState 2 since the handshake was accepted.
        _headerState = 2;
    } else
        throw std::runtime_error("WebSocket error: No WebSocket handshake"); // ws::ErrorNoHandshake
}


size_t WebSocketFramer::writeFrame(const char* data, size_t len, int flags, BitWriter& frame)
{
    assert(flags == ws::SendFlags::Text || flags == ws::SendFlags::Binary);
    assert(frame.position() == 0);
    // assert(frame.limit() >= size_t(len + MAX_HEADER_LENGTH));

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
        frame.putU64(static_cast<uint16_t>(len));
    }

    if (_maskPayload) {
        auto mask = _rnd.next();
        auto m = reinterpret_cast<const char*>(&mask);
        auto b = reinterpret_cast<const char*>(data);
        frame.put(m, 4);
        // auto p = frame.current();
        for (unsigned i = 0; i < len; i++) {
            // p[i] = b[i] ^ m[i % 4];
            frame.putU8(b[i] ^ m[i % 4]);
        }
    } else {
        // memcpy(frame.current(), data, len); // offset?
        frame.put(data, len);
    }

    // Update frame length to include payload plus header
    // frame.skip(len);

    // STrace << "Write frame: "
    //      << "\n\tinputLength: " << len
    //      << "\n\tframePosition: " << frame.position()
    //      << "\n\tframeLimit: " << frame.limit()
    //      << "\n\tframeAvailable: " << frame.available()
    //      << endl;

    return frame.position();
}


uint64_t WebSocketFramer::readFrame(BitReader& frame, char*& payload)
{
    assert(handshakeComplete());
    uint64_t limit = frame.limit();
    size_t offset = frame.position();
    // assert(offset == 0);

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
    uint64_t payloadLength = 0;
    int payloadOffset = 2;
    if ((lengthByte & 0x7f) == 127) {
        uint64_t l;
        headerReader.getU64(l);
        if (l > limit)
            throw std::runtime_error(
                util::format("WebSocket error: Insufficient buffer for payload size %" PRIu64, l)); //, ws::ErrorPayloadTooBig
        payloadLength = l;
        payloadOffset += 8;
    } else if ((lengthByte & 0x7f) == 126) {
        uint16_t l;
        headerReader.getU16(l);
        if (l > limit)
            throw std::runtime_error(util::format(
                "WebSocket error: Insufficient buffer for payload size %" PRIu64, l)); //, ws::ErrorPayloadTooBig
        payloadLength = l;
        payloadOffset += 2;
    } else {
        uint8_t l = lengthByte & 0x7f;
        if (l > limit)
            throw std::runtime_error(util::format(
                "WebSocket error: Insufficient buffer for payload size %" PRIu64, l)); //, ws::ErrorPayloadTooBig
        payloadLength = l;
    }
    if (lengthByte & FRAME_FLAG_MASK) {
        headerReader.get(mask, 4);
        payloadOffset += 4;
    }

    if (payloadLength > limit)
        throw std::runtime_error(
            "WebSocket error: Incomplete frame received"); //ws::ErrorIncompleteFrame

    // Get a reference to the start of the payload
    payload = reinterpret_cast<char*>(
        const_cast<char*>(frame.begin() + (offset + payloadOffset)));

    // Unmask the payload if required
    if (lengthByte & FRAME_FLAG_MASK) {
        auto p = reinterpret_cast<char*>(payload); // frame.data());
        for (uint64_t i = 0; i < payloadLength; i++) {
            p[i] ^= mask[i % 4];
        }
    }

    // Update frame length to include payload plus header
    frame.seek(size_t(offset + payloadOffset + payloadLength));
    // frame.limit(offset + payloadOffset + payloadLength);
    // int frameLength = (offset + payloadOffset);
    // assert(frame.position() == (offset + payloadOffset));

    return payloadLength;
}


void WebSocketFramer::completeClientHandshake(http::Response& response)
{
    assert(_mode == ws::ClientSide);
    assert(_headerState == 1);

    std::string connection = response.get("Connection", "");
    if (util::icompare(connection, "Upgrade") != 0)
        throw std::runtime_error("WebSocket error: No \"Connection: Upgrade\" header in handshake response"); //, ws::ErrorNoHandshake
    std::string upgrade = response.get("Upgrade", "");
    if (util::icompare(upgrade, "websocket") != 0)
        throw std::runtime_error("WebSocket error: No \"Upgrade: websocket\" header in handshake response"); //, ws::ErrorNoHandshake
    std::string accept = response.get("Sec-WebSocket-Accept", "");
    if (accept != computeAccept(_key))
        throw std::runtime_error("WebSocket error: Invalid or missing Sec-WebSocket-Accept header in handshake esponse"); //, ws::ErrorNoHandshake

    _headerState++;
    assert(handshakeComplete());
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
} // namespace scy


/// @\}
