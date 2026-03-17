///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "scy/http/connection.h"
#include "scy/signal.h"
#include <sstream>


namespace scy {
namespace http {


//
// HTTP Chunked Adapter
//


/// @ingroup http
/// HTTP chunked transfer encoding adapter for streaming responses.
class HTTP_API ChunkedAdapter : public IPacketizer
{
public:
    Connection::Ptr connection;
    std::string contentType;
    std::string frameSeparator;
    bool initial;
    bool nocopy;

    ChunkedAdapter(Connection::Ptr connection = nullptr, const std::string& frameSeparator = "", bool nocopy = true)
        : PacketProcessor(this->emitter)
        , connection(connection)
        , contentType(connection->outgoingHeader()->getContentType())
        , initial(true)
    {
    }

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

    /// Sets HTTP headers for the initial response.
    /// This method must not include the final carriage return.
    virtual void emitHeader()
    {
        // Flush connection headers if the connection is set.
        if (connection) {
            connection->shouldSendHeader(true);
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
    Connection::Ptr connection;
    std::string contentType;
    bool isBase64;
    bool initial;

    MultipartAdapter(Connection::Ptr connection, bool base64 = false)
        : IPacketizer(this->emitter)
        , connection(connection)
        , contentType(connection->outgoingHeader()->getContentType())
        , isBase64(base64)
        , initial(true)
    {
    }

    MultipartAdapter(const std::string& contentType, bool base64 = false)
        : IPacketizer(this->emitter)
        , connection(nullptr)
        , contentType(contentType)
        , isBase64(base64)
        , initial(true)
    {
    }

    virtual ~MultipartAdapter() {}

    virtual void emitHeader()
    {
        // Flush connection headers if the connection is set.
        if (connection) {
            connection->shouldSendHeader(true);
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

    /// Sets HTTP header for the current chunk.
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
} // namespace scy


/// @}
