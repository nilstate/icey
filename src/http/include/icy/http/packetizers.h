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

#include "icy/http/connection.h"
#include "icy/signal.h"
#include <sstream>


namespace icy {
namespace http {


//
// HTTP Chunked Adapter
//


/// @ingroup http
/// HTTP chunked transfer encoding adapter for streaming responses.
class HTTP_API ChunkedAdapter : public IPacketizer
{
public:
    Connection::Ptr connection; ///< HTTP connection to send the initial response header through.
    std::string contentType;    ///< Content-Type value for the chunked response.
    std::string frameSeparator; ///< Optional separator written before each chunk payload.
    bool initial;               ///< True until the first chunk is processed and the header emitted.
    bool nocopy;                ///< If true, header/data/footer are emitted as separate packets.

    /// Creates a ChunkedAdapter that sends its initial response header through the given connection.
    /// The content type is read from the connection's outgoing header.
    /// @param connection HTTP connection to use. May be nullptr to emit a raw response instead.
    /// @param frameSeparator Optional data prepended to each chunk payload.
    /// @param nocopy If true, header and payload are emitted as separate packets (avoids copies).
    ChunkedAdapter(Connection::Ptr connection = nullptr, const std::string& frameSeparator = "", bool nocopy = true)
        : PacketProcessor(this->emitter)
        , connection(connection)
        , contentType(connection->outgoingHeader()->getContentType())
        , initial(true)
    {
    }

    /// Creates a ChunkedAdapter that emits its own raw HTTP/1.1 200 response header.
    /// Use this when no Connection object is available.
    /// @param contentType Content-Type value for the response.
    /// @param frameSeparator Optional data prepended to each chunk payload.
    /// @param nocopy If true, header and payload are emitted as separate packets.
    ChunkedAdapter(const std::string& contentType, const std::string& frameSeparator = "", bool nocopy = true)
        : PacketProcessor(this->emitter)
        , connection(nullptr)
        , contentType(contentType)
        , frameSeparator(frameSeparator)
        , initial(true)
        , nocopy(nocopy)
    {
    }

    virtual ~ChunkedAdapter() {}

    /// Emits the initial HTTP/1.1 200 OK response headers with chunked transfer encoding.
    /// If a connection is set, headers are written through it; otherwise a raw response
    /// string is emitted via the packet signal.
    virtual void emitHeader()
    {
        // Flush connection headers if the connection is set.
        if (connection) {
            connection->beginStreaming();
            connection->setHeaderAutoSendEnabled(true);
            connection->response().setChunkedTransferEncoding(true);
            connection->response().set("Cache-Control", "no-store, no-cache, max-age=0, must-revalidate");
            connection->response().set("Cache-Control", "post-check=0, pre-check=0, FALSE");
            connection->response().set("Access-Control-Allow-Origin", "*");
            connection->response().set("Transfer-Encoding", "chunked");
            connection->response().set("Content-Type", contentType);
            connection->response().set("Connection", "keep-alive");
            connection->response().set("Pragma", "no-cache");
            connection->response().set("Expires", "0");
            connection->sendHeader();
        }
        // Otherwise make up the response.
        else {
            std::ostringstream hst;
            hst << "HTTP/1.1 200 OK\r\n"
                << "Cache-Control: no-store, no-cache, max-age=0, "
                   "must-revalidate\r\n"
                << "Cache-Control: post-check=0, pre-check=0, FALSE\r\n"
                << "Access-Control-Allow-Origin: *\r\n"
                << "Connection: keep-alive\r\n"
                << "Pragma: no-cache\r\n"
                << "Expires: 0\r\n"
                << "Transfer-Encoding: chunked\r\n"
                << "Content-Type: " << contentType << "\r\n"
                << "\r\n";
            emit(hst.str());
        }
    }

    /// Encodes an incoming packet as a chunked transfer encoding chunk and emits it.
    /// Emits the HTTP response headers on the first call.
    /// @param packet Packet containing the raw payload data.
    /// @throws std::invalid_argument if the packet does not carry data.
    virtual void process(IPacket& packet) override
    {
        LTrace("Processing:", packet.size());

        if (!packet.hasData())
            throw std::invalid_argument("Incompatible packet type");

        // Emit HTTP response header
        if (initial) {
            initial = false;
            emitHeader();
        }

        // Get hex stream length
        std::ostringstream ost;
        ost << std::hex << packet.size();

        // Emit separate packets for nocopy
        if (nocopy) {
            emit(ost.str());
            emit("\r\n", 2);
            if (!frameSeparator.empty())
                emit(frameSeparator);
            emit(packet.data(), packet.size());
            emit("\r\n", 2);
        }

        // Concat pieces for non fragmented
        else {
            ost << "\r\n";
            if (!frameSeparator.empty())
                ost << frameSeparator;
            ost.write(packet.data(), packet.size());
            ost << "\r\n";
            emit(ost.str());
        }
    }

    PacketSignal emitter;
};


//
// HTTP Multipart Adapter
//


/// @ingroup http
/// HTTP multipart encoding adapter for multipart/x-mixed-replace streaming.
class HTTP_API MultipartAdapter : public IPacketizer
{
public:
    Connection::Ptr connection; ///< HTTP connection for sending the initial response header.
    std::string contentType;    ///< Content-Type of each part (e.g. "image/jpeg").
    bool isBase64;              ///< If true, adds "Content-Transfer-Encoding: base64" to each part.
    bool initial;               ///< True until the first chunk is processed and the boundary header emitted.

    /// Creates a MultipartAdapter that sends headers through the given connection.
    /// The per-part content type is read from the connection's outgoing header.
    /// @param connection HTTP connection to use for sending the initial multipart header.
    /// @param base64 If true, indicates parts are base64-encoded.
    MultipartAdapter(Connection::Ptr connection, bool base64 = false)
        : IPacketizer(this->emitter)
        , connection(connection)
        , contentType(connection->outgoingHeader()->getContentType())
        , isBase64(base64)
        , initial(true)
    {
    }

    /// Creates a MultipartAdapter that emits its own raw HTTP/1.1 200 response header.
    /// Use this when no Connection object is available.
    /// @param contentType Content-Type for each multipart part.
    /// @param base64 If true, indicates parts are base64-encoded.
    MultipartAdapter(const std::string& contentType, bool base64 = false)
        : IPacketizer(this->emitter)
        , connection(nullptr)
        , contentType(contentType)
        , isBase64(base64)
        , initial(true)
    {
    }

    virtual ~MultipartAdapter() {}

    /// Emits the initial HTTP/1.1 200 OK response with Content-Type multipart/x-mixed-replace.
    /// If a connection is set, headers are written through it; otherwise a raw response string is emitted.
    virtual void emitHeader()
    {
        // Flush connection headers if the connection is set.
        if (connection) {
            connection->beginStreaming();
            connection->setHeaderAutoSendEnabled(true);
            connection->response().set("Content-Type", "multipart/x-mixed-replace; boundary=end");
            connection->response().set("Cache-Control", "no-store, no-cache, max-age=0, must-revalidate");
            connection->response().set("Cache-Control", "post-check=0, pre-check=0, FALSE");
            connection->response().set("Access-Control-Allow-Origin", "*");
            connection->response().set("Transfer-Encoding", "chunked");
            connection->response().set("Connection", "keep-alive");
            connection->response().set("Pragma", "no-cache");
            connection->response().set("Expires", "0");
            connection->sendHeader();
        }
        // Otherwise make up the response.
        else {
            std::ostringstream hst;
            hst << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: multipart/x-mixed-replace; boundary=end\r\n"
                << "Cache-Control: no-store, no-cache, max-age=0, "
                   "must-revalidate\r\n"
                << "Cache-Control: post-check=0, pre-check=0, FALSE\r\n"
                << "Access-Control-Allow-Origin: *\r\n"
                << "Pragma: no-cache\r\n"
                << "Expires: 0\r\n"
                << "\r\n";
            emit(hst.str());
        }
    }

    /// Emits the MIME boundary and per-part headers (Content-Type, optionally
    /// Content-Transfer-Encoding) for the next multipart chunk.
    virtual void emitChunkHeader()
    {
        // Write the chunk header
        std::ostringstream hst;

        hst << "--end\r\n"
            << "Content-Type: " << contentType << "\r\n";
        if (isBase64)
            hst << "Content-Transfer-Encoding: base64\r\n";
        hst << "\r\n";

        emit(hst.str());
    }

    /// Wraps the incoming packet as a multipart chunk and emits it downstream.
    /// Emits the multipart HTTP response headers on the first call.
    /// @param packet Packet containing the raw payload data.
    virtual void process(IPacket& packet)
    {
        // Write the initial HTTP response header
        if (initial) {
            initial = false;
            emitHeader();
        }

        // Broadcast the HTTP header separately
        // so we don't need to copy any data.
        emitChunkHeader();

        // Proxy the input packet.
        emit(packet);
    }

    PacketSignal emitter;
};


} // namespace http
} // namespace icy


/// @}
