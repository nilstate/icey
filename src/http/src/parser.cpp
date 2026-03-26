///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/parser.h"
#include "icy/crypto/crypto.h"
#include "icy/http/connection.h"
#include "icy/http/util.h"
#include "icy/logger.h"
#include <utility>
#include <stdexcept>


namespace icy {
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


Parser::ParseResult Parser::parse(const char* data, size_t len)
{
    ParseResult result;
    result.bytesConsumed = len;

    // Note: _complete may be true on keep-alive connections between requests.
    // llhttp handles this correctly - on_message_begin fires when new data
    // arrives, which calls reset() and clears _complete.

    llhttp_errno_t err = llhttp_execute(&_parser, data, len);

    if (err == HPE_PAUSED_UPGRADE) {
        // The parser stopped after the HTTP headers. Return the number
        // of bytes consumed so the caller can forward any trailing data
        // (e.g. the first WebSocket frame coalesced in the same read).
        result.bytesConsumed = static_cast<size_t>(llhttp_get_error_pos(&_parser) - data);
        llhttp_resume_after_upgrade(&_parser);
    } else if (err != HPE_OK) {
        LWarn("HTTP parse failed: ", llhttp_errno_name(err));

        // Handle error. Usually just close the connection.
        onError(err);

        const char* errorPos = llhttp_get_error_pos(&_parser);
        if (errorPos && errorPos >= data)
            result.bytesConsumed = static_cast<size_t>(errorPos - data);
    }

    result.messageComplete = _complete;
    result.upgrade = _upgrade;
    result.error = _error;
    _lastResult = result;
    return result;
}


void Parser::clearBoundMessage()
{
    if (_request) {
        _request->clear();
        _request->setMethod("");
        _request->setURI("");
        _request->setVersion(http::Message::HTTP_1_1);
    }
    if (_response) {
        _response->clear();
        _response->setVersion(http::Message::HTTP_1_1);
        _response->setStatus(http::StatusCode::OK);
        _response->setReason(http::getStatusCodeReason(http::StatusCode::OK));
    }
}


void Parser::storeHeader(std::string name, std::string value)
{
    _scratch.headers.emplace_back(std::move(name), std::move(value));
    if (_observer) {
        const auto& header = _scratch.headers.back();
        _observer->onParserHeader(header.first, header.second);
    }
}


void Parser::applyScratchToBoundMessage()
{
    if (_request) {
        _request->clear();
        _request->setVersion(_scratch.version);
        _request->setMethod(_scratch.method);
        _request->setURI(std::move(_scratch.uri));
        for (auto& [name, value] : _scratch.headers)
            _request->add(std::move(name), std::move(value));
        return;
    }

    if (_response) {
        _response->clear();
        _response->setVersion(_scratch.version);
        _response->setStatusAndReason(
            _scratch.status,
            _scratch.reason.empty()
                ? http::getStatusCodeReason(_scratch.status)
                : _scratch.reason);
        for (auto& [name, value] : _scratch.headers)
            _response->add(std::move(name), std::move(value));
    }
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
    _lastResult = {};
    _lastHeaderField.clear();
    _lastHeaderValue.clear();
    _wasHeaderValue = false;
    _scratch.reset();
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



void Parser::onHeader(std::string name, std::string value)
{
    storeHeader(std::move(name), std::move(value));
}


void Parser::onHeadersEnd()
{
    _upgrade = llhttp_get_upgrade(&_parser) != 0;

    applyScratchToBoundMessage();

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
           << llhttp_get_error_reason(&_parser);

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

    self->clearBoundMessage();

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

    self->_scratch.uri.append(at, len);
    return 0;
}


int Parser::on_status(llhttp_t* parser, const char* at, size_t length)
{
    auto self = static_cast<Parser*>(parser->data);
    if (!self) {
        return -1;
    }

    if (at && length)
        self->_scratch.reason.append(at, length);
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

    auto major = llhttp_get_http_major(&self->_parser);
    auto minor = llhttp_get_http_minor(&self->_parser);
    self->_scratch.version = (major == 1 && minor == 0)
        ? http::Message::HTTP_1_0
        : http::Message::HTTP_1_1;

    if (self->_request)
        self->_scratch.method = llhttp_method_name(
            static_cast<llhttp_method_t>(llhttp_get_method(&self->_parser)));
    if (self->_response)
        self->_scratch.status = static_cast<http::StatusCode>(
            llhttp_get_status_code(&self->_parser));

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
} // namespace icy


/// @}
