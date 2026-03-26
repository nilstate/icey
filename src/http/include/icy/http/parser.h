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

#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/net/socket.h"
#include <llhttp.h>

#include <vector>


namespace icy {
namespace http {


/// @ingroup http
/// Abstract observer interface for HTTP parser events.
class HTTP_API ParserObserver
{
public:
    /// Called for each parsed HTTP header name/value pair.
    /// @param name Header field name.
    /// @param value Header field value.
    virtual void onParserHeader(const std::string& name, const std::string& value) = 0;

    /// Called when all HTTP headers have been parsed.
    /// @param upgrade True if the connection should be upgraded (e.g. to WebSocket).
    virtual void onParserHeadersEnd(bool upgrade) = 0;

    /// Called for each chunk of body data received.
    /// @param data Pointer to the body data chunk.
    /// @param len Length of the chunk in bytes.
    virtual void onParserChunk(const char* data, size_t len) = 0;

    /// Called when the HTTP message is fully parsed.
    virtual void onParserEnd() = 0;

    /// Called when a parse error occurs.
    /// @param err Error details from llhttp.
    virtual void onParserError(const Error& err) = 0;
};


/// @ingroup http
/// HTTP request/response parser using the llhttp library.
class HTTP_API Parser
{
public:
    struct ParseResult
    {
        size_t bytesConsumed = 0;
        bool messageComplete = false;
        bool upgrade = false;
        Error error;

        [[nodiscard]] bool ok() const { return !error.any(); }
    };

    /// Creates a response parser. The response object is populated as data is parsed.
    /// @param response HTTP response object to populate.
    Parser(http::Response* response);

    /// Creates a request parser. The request object is populated as data is parsed.
    /// @param request HTTP request object to populate.
    Parser(http::Request* request);

    /// Creates a parser of the given type without binding a message object.
    /// @param type Either HTTP_REQUEST or HTTP_RESPONSE.
    Parser(llhttp_type_t type);
    ~Parser();

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;

    /// Feeds a buffer of raw HTTP data into the parser.
    ///
    /// May be called multiple times for streaming data. The parser state
    /// persists between calls. On completion or error, the observer is notified.
    /// @param data Pointer to the input data buffer.
    /// @param length Number of bytes in the buffer.
    /// @return Structured parse result including bytes consumed and terminal state.
    ParseResult parse(const char* data, size_t length);

    /// Reset the internal state (reinitialises llhttp).
    /// Safe to call externally, NOT from inside llhttp callbacks.
    void reset();

    /// Reset internal flags without reinitialising llhttp.
    /// Safe to call from inside llhttp callbacks (e.g. on_message_begin).
    void resetState();

    /// Returns true if parsing is complete, either
    /// in success or error.
    [[nodiscard]] bool complete() const;

    /// Returns true if the connection should be upgraded.
    [[nodiscard]] bool upgrade() const;

    /// Returns the parser type (HTTP_REQUEST or HTTP_RESPONSE).
    [[nodiscard]] llhttp_type_t type() const { return _type; }

    /// Binds an HTTP request object to populate during parsing.
    /// Must only be called when no message is currently set and type is HTTP_REQUEST.
    /// @param request The request object to populate.
    void setRequest(http::Request* request);

    /// Binds an HTTP response object to populate during parsing.
    /// Must only be called when no message is currently set and type is HTTP_RESPONSE.
    /// @param response The response object to populate.
    void setResponse(http::Response* response);

    /// Sets the observer that receives parser events.
    /// @param observer Observer to notify. May be nullptr to clear.
    void setObserver(ParserObserver* observer);

    /// Clear request/response pointers so they can be re-set.
    /// Used when resetting a pooled connection for reuse.
    void clearMessage();

    /// Returns the currently bound message (request or response), or nullptr.
    [[nodiscard]] http::Message* message();

    /// Returns the current parser observer, or nullptr if none is set.
    [[nodiscard]] ParserObserver* observer() const;

protected:
    struct MessageScratch
    {
        std::string version;
        std::string method;
        std::string uri;
        std::string reason;
        http::StatusCode status = http::StatusCode::OK;
        std::vector<std::pair<std::string, std::string>> headers;

        void reset()
        {
            version = http::Message::HTTP_1_1;
            method.clear();
            uri.clear();
            reason.clear();
            status = http::StatusCode::OK;
            headers.clear();
        }
    };

    void init();
    void clearBoundMessage();
    void storeHeader(std::string name, std::string value);
    void applyScratchToBoundMessage();

    /// Callbacks
    void onHeader(std::string name, std::string value);
    void onHeadersEnd();
    void onBody(const char* buf, size_t len);
    void onMessageEnd();
    void onError(llhttp_errno_t errnum, const std::string& message = "");

    /// llhttp callbacks
    static int on_message_begin(llhttp_t* parser);
    static int on_url(llhttp_t* parser, const char* at, size_t len);
    static int on_status(llhttp_t* parser, const char* at, size_t len);
    static int on_header_field(llhttp_t* parser, const char* at, size_t len);
    static int on_header_value(llhttp_t* parser, const char* at, size_t len);
    static int on_headers_complete(llhttp_t* parser);
    static int on_body(llhttp_t* parser, const char* at, size_t len);
    static int on_message_complete(llhttp_t* parser);

protected:
    ParserObserver* _observer;
    http::Request* _request;
    http::Response* _response;
    http::Message* _message;

    llhttp_t _parser;
    llhttp_settings_t _settings;
    llhttp_type_t _type;

    bool _wasHeaderValue;
    std::string _lastHeaderField;
    std::string _lastHeaderValue;

    bool _complete;
    bool _upgrade;

    Error _error;
    ParseResult _lastResult;
    MessageScratch _scratch;
};


} // namespace http
} // namespace icy


/// @}
