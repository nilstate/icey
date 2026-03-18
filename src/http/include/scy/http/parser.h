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

#include "scy/http/request.h"
#include "scy/http/response.h"
#include "scy/net/socket.h"
#include <llhttp.h>


namespace scy {
namespace http {


/// @ingroup http
/// Abstract observer interface for HTTP parser events.
class HTTP_API ParserObserver
{
public:
    virtual void onParserHeader(const std::string& name, const std::string& value) = 0;
    virtual void onParserHeadersEnd(bool upgrade) = 0;
    virtual void onParserChunk(const char* data, size_t len) = 0;
    virtual void onParserEnd() = 0;

    virtual void onParserError(const Error& err) = 0;
};


/// @ingroup http
/// HTTP request/response parser using the llhttp library.
class HTTP_API Parser
{
public:
    Parser(http::Response* response);
    Parser(http::Request* request);
    Parser(llhttp_type_t type);
    ~Parser();

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;

    /// Parse a HTTP packet.
    ///
    /// Returns true of the message is complete, false if incomplete.
    /// Reset the parser state for a new message
    size_t parse(const char* data, size_t length);

    /// Reset the internal state.
    void reset();

    /// Returns true if parsing is complete, either
    /// in success or error.
    [[nodiscard]] bool complete() const;

    /// Returns true if the connection should be upgraded.
    [[nodiscard]] bool upgrade() const;

    void setRequest(http::Request* request);
    void setResponse(http::Response* response);
    void setObserver(ParserObserver* observer);

    [[nodiscard]] http::Message* message();
    [[nodiscard]] ParserObserver* observer() const;

protected:
    void init();

    /// Callbacks
    void onURL(const std::string& value);
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
};


} // namespace http
} // namespace scy


/// @}
