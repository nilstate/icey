///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/parser.h"
#include "scy/crypto/crypto.h"
#include "scy/http/connection.h"
#include "scy/http/util.h"
#include "scy/logger.h"
#include <stdexcept>


namespace scy {
namespace http {


Parser::Parser(http::Response* response)
    : _observer(nullptr)
    , _request(nullptr)
    , _response(response)
    , _type(HTTP_RESPONSE)
{
    init();
    reset();
}


Parser::Parser(http::Request* request)
    : _observer(nullptr)
    , _request(request)
    , _response(nullptr)
    , _type(HTTP_REQUEST)
{
    init();
    reset();
}


Parser::Parser(llhttp_type_t type)
    : _observer(nullptr)
    , _request(nullptr)
    , _response(nullptr)
    , _type(type)
{
    init();
    reset();
}


Parser::~Parser()
{
}


void Parser::init()
{
    llhttp_settings_init(&_settings);
    _settings.on_message_begin = on_message_begin;
    _settings.on_url = on_url;
    _settings.on_status = on_status;
    _settings.on_header_field = on_header_field;
    _settings.on_header_value = on_header_value;
    _settings.on_headers_complete = on_headers_complete;
    _settings.on_body = on_body;
    _settings.on_message_complete = on_message_complete;

    llhttp_init(&_parser, _type, &_settings);
    _parser.data = this;

    reset();
}


size_t Parser::parse(const char* data, size_t len)
{
    // Note: _complete may be true on keep-alive connections between requests.
    // llhttp handles this correctly - on_message_begin fires when new data
    // arrives, which calls reset() and clears _complete.

    llhttp_errno_t err = llhttp_execute(&_parser, data, len);

    if (err == HPE_PAUSED_UPGRADE) {
        // The parser has only parsed the HTTP headers, there
        // may still be unread data from the request body in the buffer.
        llhttp_resume_after_upgrade(&_parser);
    } else if (err != HPE_OK) {
        LWarn("HTTP parse failed: ", llhttp_errno_name(err));

        // Handle error. Usually just close the connection.
        onError(err);
    }

    return len;
}


void Parser::reset()
{
    llhttp_init(&_parser, _type, &_settings);
    _parser.data = this;
    resetState();
}


void Parser::resetState()
{
    _complete = false;
    _upgrade = false;
    _error.reset();
    _lastHeaderField.clear();
    _lastHeaderValue.clear();
    _wasHeaderValue = false;
}


void Parser::clearMessage()
{
    _request = nullptr;
    _response = nullptr;
    _message = nullptr;
}


void Parser::setRequest(http::Request* request)
{
    if (_request || _response || _type != HTTP_REQUEST) {
        throw std::runtime_error("Parser::setRequest: invalid state");
    }
    _request = request;
}


void Parser::setResponse(http::Response* response)
{
    if (_request || _response || _type != HTTP_RESPONSE) {
        throw std::runtime_error("Parser::setResponse: invalid state");
    }
    _response = response;
}


void Parser::setObserver(ParserObserver* observer)
{
    _observer = observer;
}


http::Message* Parser::message()
{
    return _request    ? static_cast<http::Message*>(_request)
           : _response ? static_cast<http::Message*>(_response)
                       : nullptr;
}


ParserObserver* Parser::observer() const
{
    return _observer;
}


bool Parser::complete() const
{
    return _complete;
}


bool Parser::upgrade() const
{
    return _upgrade;
}


//
// Callbacks


void Parser::onURL(const std::string& value)
{
    if (_request)
        _request->setURI(value);
}


void Parser::onHeader(std::string name, std::string value)
{
    if (_observer)
        _observer->onParserHeader(name, value);
    if (message())
        message()->add(std::move(name), std::move(value));
}


void Parser::onHeadersEnd()
{
    _upgrade = llhttp_get_upgrade(&_parser) != 0;

    if (_observer)
        _observer->onParserHeadersEnd(_upgrade);
}


void Parser::onBody(const char* buf, size_t len)
{
    if (_observer)
        _observer->onParserChunk(buf, len);
}


void Parser::onMessageEnd()
{
    _complete = true;
    if (_observer)
        _observer->onParserEnd();
}


void Parser::onError(llhttp_errno_t errnum, const std::string& message)
{
    if (errnum == HPE_OK) {
        throw std::logic_error("Parser::onError called with HPE_OK");
    }
    SDebug << "Parse error: "
           << llhttp_errno_name(errnum) << ": "
           << llhttp_get_error_reason(&_parser) << std::endl;

    _complete = true;
    _error.err = static_cast<int>(errnum);
    _error.message = message.empty() ? llhttp_errno_name(errnum) : message;
    if (_observer)
        _observer->onParserError(_error);
}


//
// llhttp callbacks
//


int Parser::on_message_begin(llhttp_t* parser)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    // Clear request/response state for keep-alive (next message on same connection)
    if (self->_request) {
        self->_request->clear();
        self->_request->setMethod("");
        self->_request->setURI("");
        self->_request->setVersion(http::Message::HTTP_1_1);
    }
    if (self->_response) {
        self->_response->clear();
        self->_response->setStatus(http::StatusCode::OK);
        self->_response->setReason(http::getStatusCodeReason(http::StatusCode::OK));
        self->_response->setVersion(http::Message::HTTP_1_1);
    }

    // Use resetState() instead of reset() - we're inside an llhttp callback,
    // so we must NOT call llhttp_init() which would corrupt the parser.
    self->resetState();
    return 0;
}


int Parser::on_url(llhttp_t* parser, const char* at, size_t len)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self || !at || !len) {
        return -1;
    }

    // Use a thread-local to avoid per-callback allocation.
    // llhttp may deliver the URL across multiple on_url calls,
    // but in practice it arrives in one piece for typical requests.
    thread_local std::string url;
    url.assign(at, len);
    self->onURL(url);
    return 0;
}


int Parser::on_status(llhttp_t* parser, const char* at, size_t length)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    // Handle response status line
    if (self->_response)
        self->_response->setStatus(static_cast<http::StatusCode>(llhttp_get_status_code(&self->_parser)));

    return 0;
}


int Parser::on_header_field(llhttp_t* parser, const char* at, size_t len)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    if (self->_wasHeaderValue) {
        if (!self->_lastHeaderField.empty()) {
            self->onHeader(std::move(self->_lastHeaderField), std::move(self->_lastHeaderValue));
            self->_lastHeaderValue.clear();
        }
        self->_lastHeaderField.assign(at, len);
        self->_wasHeaderValue = false;
    } else {
        self->_lastHeaderField.append(at, len);
    }

    return 0;
}


int Parser::on_header_value(llhttp_t* parser, const char* at, size_t len)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    if (!self->_wasHeaderValue) {
        self->_lastHeaderValue.assign(at, len);
        self->_wasHeaderValue = true;
    } else {
        self->_lastHeaderValue.append(at, len);
    }

    return 0;
}


int Parser::on_headers_complete(llhttp_t* parser)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    // Add last entry if any
    if (!self->_lastHeaderField.empty()) {
        self->onHeader(std::move(self->_lastHeaderField), std::move(self->_lastHeaderValue));
    }

    // Request HTTP method
    if (self->_request)
        self->_request->setMethod(llhttp_method_name(
            static_cast<llhttp_method_t>(llhttp_get_method(&self->_parser))));

    self->onHeadersEnd();
    return 0;
}


int Parser::on_body(llhttp_t* parser, const char* at, size_t len)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    self->onBody(at, len);
    return 0;
}


int Parser::on_message_complete(llhttp_t* parser)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    // Signal message complete when the llhttp
    // has finished receiving the message.
    self->onMessageEnd();
    return 0;
}


} // namespace http
} // namespace scy


/// @}
